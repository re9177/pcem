#include "netbackend.h"
#include <stdlib.h>

extern void null_add();
extern void tap_add();

static network_backend_t* backends[16];
static int backend_count = 0;

void network_backend_init() {
	null_add();
	// slirp_add();
	tap_add();
}

void network_backend_add(network_backend_t* backend) {
	backends[backend_count++] = backend;
}

int network_backend_get_count() {
	return backend_count;
}

network_backend_t* network_backend_get(int index) {
	// TODO assert
	return backends[index];
}

// Creates an instance of a network backend.
void* network_backend_create_instance(network_backend_t* backend, const network_adapter_t* adapter) {
	void* instance = malloc(backend->implsize);
	if(instance == NULL)
		return NULL;

	if(!backend->initialize(instance, adapter)) {
		backend->shutdown(instance);
		return NULL;
	}

	return instance;
}

void network_backend_destroy_instance(void* instance) {
	// shutdown was expected
	if(instance)
		free(instance);
}
