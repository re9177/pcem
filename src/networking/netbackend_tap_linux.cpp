#include "netbackend.h"
#include "netpacket.hpp"
#include <cstdint>
#include <cstring>
#include <new>
#include <queue>
#include <string>

#include <unistd.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include <poll.h>

extern "C" {
    #include "logging.h"
}

constexpr static auto MAX_PACKET_QUEUE_LENGTH = 64;


// This class hides away most of the management of the tun device
class TunManager {
    int tunFd;

    void openTun() {
        // Don't try and re-open the TUN file descriptor
        // if one has already successfully been opened.
        if(tunFd > 0)
            return;

        tunFd = ::open("/dev/net/tun", O_RDWR|O_CLOEXEC);
        if(tunFd < 0) {
            pclog("DEV9: Failed to open /dev/net/tun. TAP networking will not function.");
            tunFd = -1;
            return;
        }
    }

    void closeTun() {
        if(tunFd > 0) {
            close(tunFd);
            tunFd = -1;
        }
    }

    static struct ifreq makeInterfaceRequest(const std::string& tapDeviceName, int flags = 0) {
        struct ifreq interfaceRequest{};
        interfaceRequest.ifr_flags = flags;
        std::strncpy(&interfaceRequest.ifr_name[0], tapDeviceName.data(), IFNAMSIZ);
        return interfaceRequest;
    }

public:

    int openTap(const std::string& tapDeviceName) {
        // Try and open the TUN device. If that fails, give up.
        if(openTun(); tunFd == -1)
            return -1;

        // Setup the tun request to request a TAP device.
        // Do not require or inject the legacy TAP header.
        auto interfaceRequest = makeInterfaceRequest(tapDeviceName, IFF_TAP | IFF_NO_PI);

        // Request the TAP device from the TUN.
        int result = ioctl(tunFd, TUNSETIFF, &interfaceRequest);
        if(result == -1) {
            pclog("Could not request TAP %s from /dev/net/tun. TAP networking will not function.", tapDeviceName.c_str());
            return -1;
        }

        // Enable non-blocking
        fcntl(tunFd, F_SETFL, fcntl(tunFd, F_GETFL, 0) | O_NONBLOCK);

        return tunFd;
    }

    void closeTap() {
        closeTun();
    }
};


class TapNetwork {
    TunManager tunManager;

    // queue of packets recieved
    std::queue<NetPacket*> rxQueue;

    std::queue<NetPacket*> txQueue;

    // Default to cable connected
    bool cableConnected{true};

    int fd;
    struct pollfd pollState;

    void queueReadPacket() {
        auto* packet = net_createPacket(2000);
        if(!packet) {
            fatal("net_createPacket failed");
        }

        int nRead = read(fd, packet->getData(), packet->getSize());
        // Ignore invalid reads.
        if(nRead < 0 || nRead == 0) {
            net_returnPacket(packet);
            return;
        }

        // Packet recieved, add it to queue
        packet->resizeTo(nRead);
        rxQueue.push(packet);
    }

    void writeQueuedPacket() {
        // Don't bother
        if(txQueue.empty())
            return;

        auto packet = txQueue.front();

        // pop packet
        net_returnPacket(packet);
        txQueue.pop();
    }

    void dropQueues() {

    }

public:

    int init(const network_adapter_t* adapter) {
        if(!adapter)
            return 1;

        fd = tunManager.openTap(adapter->name);
        if(fd == -1) {
            pclog("Failed to open interface %s.", adapter->name);
            return 1;
        }

        pollState.fd = fd;
        pollState.events = POLLIN | POLLOUT;
        return 0;
    }

    void shutdown() {
        if(fd != -1) {
            tunManager.closeTap();
        }
        return;
    }

    void setCableConnectState(int connected) {
        if(connected)
            cableConnected = true;
        else {
            cableConnected = false;
            // TODO: Drop all packets in rx and tx queues?
            // We need to call net_returnPacket on them all
        }
    }

    int getCableConnectState() {
        if(cableConnected)
            return 1;
        return 0;
    }

    void pollEvents() {
        int pollStatus = poll(&pollState, 1, 100);
        if(pollStatus == 0)
            return;

        if(pollStatus > 0) {
            // Depending on the events which were raised,
            // we can now react to them and do things.

            if(pollState.revents & POLLIN) {
                queueReadPacket();
            }

            if(pollState.revents & POLLOUT) {
                writeQueuedPacket();
            }
        } else {
            pclog("Error polling for events.");
        }
    }

    int sendPacket(const uint8_t* buffer, size_t size) {
        // Don't bother
        if(!cableConnected)
            return -1;

        auto* packet = net_createPacket(size);
        if(!packet) {
            fatal("Network backend out of packets??");
        }

        // If we go above the packet queue length we are probably
        // somehow stalled, so dropping the packet seems like the best option.
        if(txQueue.size() + 1 > MAX_PACKET_QUEUE_LENGTH)
            return -1;

        // Clone packet and push it. pollEvents() will write it.
        memcpy(packet->getData(), buffer, size);
        txQueue.push(packet);
        return 0;
    }

    int recievePacket(uint8_t* buffer, size_t size) {
        // No packets are available to read.
        if(rxQueue.empty()) {
            return -1;
        }

        if(!cableConnected)
            return -1;

        auto packet = rxQueue.front();

        // Copy packet to user buffer
        memcpy(&buffer[0], packet->getData(), packet->getSize());

        // pop it
        net_returnPacket(packet);
        rxQueue.pop();
        return packet->getSize(); // packet read.
    }
};

static network_backend_t net_tap = {
    .name = "Linux TAP",
    .id = "tap",
    .implsize = sizeof(TapNetwork),
    .requiresAdapterBinding = 1,
    .getAdapterList = [](size_t* pcount) -> const network_adapter_t* {
        return nullptr;
    },

    .initialize = [](void* pimpl, const network_adapter_t* adapter) -> int {
        auto* p = new (pimpl) TapNetwork;
        return p->init(adapter);
    },

    .shutdown = [](void* pimpl) -> void {
        auto* p = reinterpret_cast<TapNetwork*>(pimpl);
        p->shutdown();
        p->~TapNetwork();
    },

    .setCableConnectState = [](void* pimpl, int connected) -> void {
        return reinterpret_cast<TapNetwork*>(pimpl)->setCableConnectState(connected);
    },

    .getCableConnectState = [](void* pimpl) -> int {
        return reinterpret_cast<TapNetwork*>(pimpl)->getCableConnectState();
    },

    .doPoll = [](void* pimpl) {
        return reinterpret_cast<TapNetwork*>(pimpl)->pollEvents();
    },

    .sendPacket = [](void* pimpl, const uint8_t* buffer, size_t size) -> int {
        return reinterpret_cast<TapNetwork*>(pimpl)->sendPacket(buffer, size);
    },

    .recievePacket = [](void* pimpl, uint8_t* buffer, size_t size) -> int {
        return reinterpret_cast<TapNetwork*>(pimpl)->recievePacket(buffer, size);
    },
};

extern "C" void tap_add() {
    network_backend_add(&net_tap);
}
