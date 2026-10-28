#include "instancebase.hpp"
extern "C" {
	#include "logging.h"
}

/// Drops all packets, hopefully freeing the memory.
void NetworkInstance::dropQueues() {
	while(!rxQueue.empty()) {
		net_returnPacket(rxQueue.front());
		rxQueue.pop();
	}

	while(!txQueue.empty()) {
		net_returnPacket(txQueue.front());
		txQueue.pop();
	}
}

void NetworkInstance::pushRxQueue(NetPacket* packet) {
	if(rxQueue.size() + 1 > MAX_PACKET_QUEUE_LENGTH) {
		net_returnPacket(packet);
		return;
	}

	rxQueue.push(packet);
}

NetPacket* NetworkInstance::peekTxQueue() {
	if(txQueue.empty())
		return nullptr;
	return txQueue.front();
}

void NetworkInstance::popTxQueue() {
	if(txQueue.empty())
		return;

	net_returnPacket(txQueue.front());
	txQueue.pop();
}

void NetworkInstance::setCableConnectState(int connected) {
	if(connected)
		cableConnected = true;
	else {
		cableConnected = false;
		// Drop all in-flight packets
		dropQueues();
	}
}

int NetworkInstance::getCableConnectState() {
	if(cableConnected)
		return 1;
	return 0;
}

int NetworkInstance::sendPacket(const uint8_t* buffer, size_t size) {
	// Don't bother
	if(!cableConnected)
		return -1;

	auto* packet = net_createPacket(size);
	if(!packet) {
		fatal("net_createPacket failed");
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

int NetworkInstance::recievePacket(uint8_t* buffer, size_t size) {
	if(!cableConnected)
		return -1;

	// No packets are available to read currently.
	if(rxQueue.empty()) {
		return -1;
	}

	auto packet = rxQueue.front();

	// TODO should we re-enqueue a packet, or if you dont read a full one
	// you're screwed?
	const auto actuallyReadSize = std::min(size, packet->getSize());

	// Copy packet to user-provided buffer.
	memcpy(&buffer[0], packet->getData(), actuallyReadSize);

	// pop it
	net_returnPacket(packet);
	rxQueue.pop();
	return packet->getSize(); // packet read.
}
