#pragma once
#include <cstdint>
#include <cstddef>

/// A network packet.
class NetPacket {
	std::uint8_t buffer[2000];
	size_t packetSize;
   public:

	NetPacket() = default;

	size_t getSize() const {
		return packetSize;
	}

	// Resizes a packet to a smaller length.
	void resizeTo(size_t newSize) {
		if(newSize > sizeof(buffer))
			return;
		// Resize.
		packetSize = newSize;
	}

	uint8_t* getData() {
		return reinterpret_cast<uint8_t*>(&buffer[0]);
	}

	const uint8_t* getData() const {
		return reinterpret_cast<const uint8_t*>(&buffer[0]);
	}
};

// Creates a packet with the given size. May actually re-use
// old packet objects.
NetPacket* net_createPacket(size_t packetSize);

// Returns packet to the freelist.
void net_returnPacket(NetPacket* packet);

// Deletes all packets. Only call at shutdown.
void net_destroyAllPackets();
