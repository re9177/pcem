#ifndef PLATFORM_SOUND_DRIVER_H
#define PLATFORM_SOUND_DRIVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PLATFORM_CD_FREQ 44100
#define PLATFORM_CD_BUFFER_LENGTH (PLATFORM_CD_FREQ / 10)

#define PLATFORM_FREQ 48000
#define PLATFORM_MAX_SOUND_BUFFER_LENGTH (PLATFORM_FREQ / 10)

#if 0
// TODO, this is how I want sound drivers to be exposed.
// Later on once the codebase is fully C++ we can use
// virtual functions which will be a lot more ergonomic.
typedef struct {
	const char* name;
	const char* id;

	void(*init)(int sampleRate);

	int (*getBufferLength)();

	void(*setBufferLength)(int newBufferLength);

	void(*shutdown)();

	void(*giveBuffer)();

	void(*giveBufferCD)();
} sound_driver_t;
#endif

// This is the current static api used by
// the touched up OpenAL code.

void sound_driver_init();
void sound_driver_shutdown();

int sound_driver_get_buffer_length();
void sound_driver_set_buffer_length(int newBufferLength);

int sound_driver_get_gain();
void sound_driver_set_gain(int newGain);

// NOTE: buf[] is expected to be valid up to [sound_driver_get_buffer_length()].
void sound_driver_write(int32_t* buf);
void sound_driver_write_cd(int16_t* buf);


#ifdef __cplusplus
}
#endif

#endif
