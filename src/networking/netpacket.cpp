
#include "networking/netpacket.hpp"
#include "logging.h"
#include <cstdlib>
#include <new>

// TODO: Static/locked free list of NetPacket*s,
// so we re-use them.

NetPacket* net_createPacket(size_t packetSize) {
    void* packetmem = malloc(sizeof(NetPacket) + packetSize);
    if(packetmem == nullptr)
        return nullptr;
    return new (packetmem) NetPacket;
}

void net_returnPacket(NetPacket* packet) {
    if(packet) {
        packet->~NetPacket();
        free(packet);
    }
}

void net_destroyAllPackets() {
    pclog("net_destroyAllPackets stub!");
    return;
}
