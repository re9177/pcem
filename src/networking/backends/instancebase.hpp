#include <queue>
#include <cstring>
#include "networking/netpacket.hpp"

constexpr static auto MAX_PACKET_QUEUE_LENGTH = 256;

/// Base class for fully-featured network backends.
class NetworkInstance {
	// queue of packets recieved
	std::queue<NetPacket*> rxQueue;

	std::queue<NetPacket*> txQueue;

	// Default to cable connected
	bool cableConnected { true };

protected:

	bool isCableConnected() const { return cableConnected; }

	/// Drops all packets, hopefully freeing the memory.
	void dropQueues();
	void pushRxQueue(NetPacket* packet);
	NetPacket* peekTxQueue();
	void popTxQueue();
public:

	void setCableConnectState(int connected);

	int getCableConnectState();

	int sendPacket(const uint8_t* buffer, size_t size);

	int recievePacket(uint8_t* buffer, size_t size);
};
