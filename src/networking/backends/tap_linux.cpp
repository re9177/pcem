#include "networking/netbackend.h"
#include "networking/netpacket.hpp"
#include "instancebase.hpp"
#include <cstdint>
#include <cstring>
#include <new>
#include <queue>
#include <string>

#include <unistd.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <ifaddrs.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include <poll.h>

extern "C" {
#include "logging.h"
}

// This class hides away most of the management of the tun device
class TunManager {
	int tunFd;

	void openTun() {
		// Don't try and re-open the TUN file descriptor
		// if one has already successfully been opened.
		if(tunFd > 0)
			return;

		tunFd = ::open("/dev/net/tun", O_RDWR | O_CLOEXEC);
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
		struct ifreq interfaceRequest {};
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

		// Enable non-blocking I/O.
		fcntl(tunFd, F_SETFL, fcntl(tunFd, F_GETFL, 0) | O_NONBLOCK);

		return tunFd;
	}

	/// Returns true if the provided interface name is a TAP device.
	bool isTapDevice(const std::string& tapDeviceName) {
		if(openTun(); tunFd == -1)
			return false;

		auto interfaceRequest = makeInterfaceRequest(tapDeviceName);
		if(ioctl(tunFd, TUNGETIFF, &interfaceRequest) >= 0) {
			return (interfaceRequest.ifr_flags & IFF_TAP) == IFF_TAP;
		}

		return false;
	}

	void closeTap() {
		closeTun();
	}
};

class TapNetworkInstance final : public NetworkInstance {
	TunManager tunManager;
	int fd;
	struct pollfd pollState;
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
		// Drop all packets
		dropQueues();
	}

	void pollEvents() {
		int pollStatus = poll(&pollState, 1, 100);
		if(pollStatus == 0)
			return;

		if(pollStatus > 0) {
			// Depending on the events which were raised,
			// we can now react to them and do things.

			if(pollState.revents & POLLIN) {
				// the TAP is readable, so try and recieve a packet from it.
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
				pushRxQueue(packet);
			}

			if(pollState.revents & POLLOUT) {
				// The TAP is writable, so let's write a packet which was previously queued.
				auto packet = peekTxQueue();
				if(packet == nullptr)
					return;

				int nWritten = write(fd, packet->getData(), packet->getSize());
				if(nWritten < 0 || nWritten == 0) {
					pclog("error writing tap %d\n", errno);
				}

				popTxQueue();
			}
		} else {
			pclog("Error polling for events.");
		}
	}
};

const network_adapter_t* getTapAdapters(size_t* pCount) {
	static std::vector<network_adapter_t> adapterList{};

	// simple static string pool for interface names.
	// kinda garbage but well hey so is most of the pcem code :)
	static struct NamePool {
		char pool[2048];
		char* pCurrent;

		NamePool() {
			pCurrent = &pool[0];
		}

		char* dup(const char* psz) {
			char* writeTo = pCurrent;
			auto count = strlen(psz);
			memcpy(pCurrent, psz, count);
			writeTo[count] = '\0';
			pCurrent += count+1;

			return writeTo;
		}
	} namePool{};


	pclog("TAP getTapAdapters\n");

	if(adapterList.empty()) {
		// Generate the adapter list.
		pclog("TAP Generating adapter list\n");
		struct ifaddrs* list;
		if(getifaddrs(&list) == -1) {
			if(pCount)
				*pCount = 0;
			return nullptr;
		}

		for(auto interface = list; interface != nullptr; interface = interface->ifa_next) {
			if(interface->ifa_addr)
				continue;
			TunManager tun;
			if(tun.isTapDevice(interface->ifa_name)) {
				// Add it to the list.
				network_adapter_t tempAdapter = {
					.name = namePool.dup(interface->ifa_name),
					.flags = 0
				};
				adapterList.push_back(tempAdapter);
			}
		}

		freeifaddrs(list);
	}

	if(pCount)
		*pCount = adapterList.size();
	return adapterList.data();
}

static network_backend_t net_tap = {
	.name = "Linux TAP",
	.id = "tap",
	.implsize = sizeof(TapNetworkInstance),
	.requiresAdapterBinding = 1,
	.getAdapterList = [](size_t* pcount) -> const network_adapter_t* {
		return getTapAdapters(pcount);
	},

	.initialize = [](void* pimpl, const network_adapter_t* adapter) -> int {
		auto* p = new(pimpl) TapNetworkInstance;
		return p->init(adapter);
	},

	.shutdown = [](void* pimpl) -> void {
		auto* p = reinterpret_cast<TapNetworkInstance*>(pimpl);
		p->shutdown();
		p->~TapNetworkInstance();
	},

	.setCableConnectState = [](void* pimpl, int connected) -> void {
		return reinterpret_cast<TapNetworkInstance*>(pimpl)->setCableConnectState(connected);
	},

	.getCableConnectState = [](void* pimpl) -> int {
		return reinterpret_cast<TapNetworkInstance*>(pimpl)->getCableConnectState();
	},

	.doPoll = [](void* pimpl) { return reinterpret_cast<TapNetworkInstance*>(pimpl)->pollEvents(); },

	.sendPacket = [](void* pimpl, const uint8_t* buffer, size_t size) -> int {
		return reinterpret_cast<TapNetworkInstance*>(pimpl)->sendPacket(buffer, size);
	},

	.recievePacket = [](void* pimpl, uint8_t* buffer, size_t size) -> int {
		return reinterpret_cast<TapNetworkInstance*>(pimpl)->recievePacket(buffer, size);
	},
};

extern "C" void tap_add() {
	network_backend_add(&net_tap);
}
