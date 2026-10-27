#pragma once
#include <cstdint>
#include <cstddef>

/// A network packet.
class NetPacket {
    size_t allocatedSize;
public:
    // NOTE: This member is only public for introspection.
    // The implementation uses this. Use the methods in NetPacket
    // to interact with the class.
    size_t packetSize;

    size_t getSize() const {
        return packetSize;
    }

    // Resizes a packet to a smaller length.
    void resizeTo(size_t newSize) {
        if(newSize > allocatedSize)
            return;
        // Resize.
        packetSize = newSize;
    }

    uint8_t* getData() {
        return reinterpret_cast<uint8_t*>(this + 1);
    }

    const uint8_t* getData() const {
        return reinterpret_cast<const uint8_t*>(this + 1);
    }
};

// Creates a packet with the given size. May actually re-use
// old packet objects.
NetPacket* net_createPacket(size_t packetSize);

// Returns packet to the freelist.
void net_returnPacket(NetPacket* packet);

// Deletes all packets. Only call at shutdown.
void net_destroyAllPackets();

