#include "networking/netbackend.h"

static network_backend_t net_null = {
	.name = "None",
	.id = "null",
	.instanceSize = sizeof(int),
	.requiresBinding = 0,
	.getBindingList = [](size_t* pcount) -> const network_binding_t* {
		if(pcount)
			*pcount = 0;
		return nullptr;
	},

	.initialize = [](void* pimpl, const network_binding_t* adapter) -> int {
		return 0; // success
	},

	.shutdown = [](void* pimpl) -> void {
		return;
	},

	.setCableConnectState = [](void* pimpl, int connected) -> void {
		return;
	},

	.getCableConnectState = [](void* pimpl) -> int {
		// Cable is always connected?
		return 1;
	},

	.doPoll = [](void* pimpl) {
		return; // nothing to poll
	},

	.sendPacket = [](void* pimpl, const uint8_t* buffer, size_t size) -> int {
		return 0; // pretend like packet is always queued
	},

	.recievePacket = [](void* pimpl, uint8_t* buffer, size_t size) -> int {
		return 0;
	},
};

extern "C" void null_add() {
	network_backend_add(&net_null);
}
