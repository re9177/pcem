#ifndef PCEM_NETBACKEND_H
#define PCEM_NETBACKEND_H

// In a very distant future, this file will be C++.
// That future is not now.

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	const char* name;
	int flags; // No flags are defined yet
} network_adapter_t;

typedef struct {
	const char* name;
	const char* id;

	// size of the implementation to allocate.
	size_t implsize;

	// set to 1 (todo bool true) if this backend requires binding to a specific adapter to function.
	// set to 0 otherwise in the case that it does not.
	int requiresAdapterBinding;

	// obtains a list of all viable adapters (optionally placing count in [*count]). if [requiresAdapterBinding] is false,
	// this function is permitted to return a null pointer since there is no adapter binding required, and thus
	// no need to actually populate a list of adapters.
	const network_adapter_t* (*getAdapterList)(size_t* count);

	// initalize the backend with the selected adapter. [adapter] is permitted to be a null pointer
	// when [requiresAdapterBinding] is false.
	int (*initialize)(void* pimpl, const network_adapter_t* adapter);

	// shuts down the backend.
	void (*shutdown)(void* pimpl);

	// Sets whether or not the cable is connected.
	void (*setCableConnectState)(void* pimpl, int connected /* FIXME bool */);

	int (*getCableConnectState)(void* pimpl);

	// should be called periodically.
	// returns a bitmask of STATE_* to indicate
	// the current polled state.
	void (*doPoll)(void* pimpl);

	// queues a packet for send. returns -1 if the packet queue is full
	int (*sendPacket)(void* pimpl, const uint8_t* buffer, size_t size);

	// recieves a packet. returns -1 if no packets are currently readable.
	int (*recievePacket)(void* pimpl, uint8_t* buffer, size_t size);
} network_backend_t;

void network_backend_init();

void network_backend_add(network_backend_t* backend);

int network_backend_get_count();

network_backend_t* network_backend_get(int index);

// Creates an instance of a network backend.
void* network_backend_create_instance(network_backend_t* backend, const network_adapter_t* adapter);

void network_backend_destroy_instance(void* instance);

#ifdef __cplusplus
}
#endif

#endif
