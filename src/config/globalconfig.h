#ifndef PCEM_CONFIG_GLOBALCONFIG_H
#define PCEM_CONFIG_GLOBALCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct {

	} config_global_config_t;

	// initalize and load the global configuration
	void config_global_init();

	void config_global_reload();

	void config_global_save();


#ifdef __cplusplus
	}
#endif

#endif
