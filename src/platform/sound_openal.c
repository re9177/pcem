#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include "sound.h"

FILE* allog;

ALuint buffers[4];		 // front and back buffers
ALuint buffers_cd[4];	 // front and back buffers
static ALuint source[2]; // audio source

static int driver_buffer_length = PLATFORM_FREQ / 20;
static int driver_gain = 0;

ALvoid alutInit() {
	ALCcontext* Context;
	ALCdevice* Device;

	// Open device
	// 	Device=alcOpenDevice((ALubyte*)"");
	Device = alcOpenDevice((void*)"");
	// Create context(s)
	Context = alcCreateContext(Device, NULL);
	// Set active context
	alcMakeContextCurrent(Context);
	// Register extensions
}

ALvoid alutExit(ALvoid) {
	ALCcontext* Context;
	ALCdevice* Device;

	// Unregister extensions

	// Get active context
	Context = alcGetCurrentContext();
	// Get device for active context
	Device = alcGetContextsDevice(Context);
	// Disable context
	alcMakeContextCurrent(NULL);
	// Release context(s)
	alcDestroyContext(Context);
	// Close device
	alcCloseDevice(Device);
}

void check() {
	ALenum error;
	if((error = alGetError()) != AL_NO_ERROR) {
		//                printf("Error : %08X\n", error);
		//                exit(-1);
	}
}

void sound_driver_init() {
	int c;
	int16_t buf[PLATFORM_MAX_SOUND_BUFFER_LENGTH * 2];
	int16_t cd_buf[PLATFORM_CD_BUFFER_LENGTH * 2];

	alutInit();
	check();

	alGenBuffers(4, buffers);
	check();
	alGenBuffers(4, buffers_cd);
	check();

	alGenSources(2, source);
	check();

	alSource3f(source[0], AL_POSITION, 0.0, 0.0, 0.0);
	alSource3f(source[0], AL_VELOCITY, 0.0, 0.0, 0.0);
	alSource3f(source[0], AL_DIRECTION, 0.0, 0.0, 0.0);
	alSourcef(source[0], AL_ROLLOFF_FACTOR, 0.0);
	alSourcei(source[0], AL_SOURCE_RELATIVE, AL_TRUE);
	check();
	alSource3f(source[1], AL_POSITION, 0.0, 0.0, 0.0);
	alSource3f(source[1], AL_VELOCITY, 0.0, 0.0, 0.0);
	alSource3f(source[1], AL_DIRECTION, 0.0, 0.0, 0.0);
	alSourcef(source[1], AL_ROLLOFF_FACTOR, 0.0);
	alSourcei(source[1], AL_SOURCE_RELATIVE, AL_TRUE);
	check();

	memset(buf, 0, driver_buffer_length * 4);
	memset(cd_buf, 0, PLATFORM_CD_BUFFER_LENGTH * 4);

	for(c = 0; c < 4; c++) {
		alBufferData(buffers[c], AL_FORMAT_STEREO16, buf, driver_buffer_length * 2 * 2, PLATFORM_FREQ);
		alBufferData(buffers_cd[c], AL_FORMAT_STEREO16, cd_buf, PLATFORM_CD_BUFFER_LENGTH * 2 * 2, PLATFORM_CD_FREQ);
	}

	alSourceQueueBuffers(source[0], 4, buffers);
	check();
	alSourceQueueBuffers(source[1], 4, buffers_cd);
	check();

	alSourcePlay(source[0]);
	check();
	alSourcePlay(source[1]);
	check();
}

void sound_driver_shutdown() {
	alutExit();
}

int sound_driver_get_buffer_length() {
	return driver_buffer_length;
}

void sound_driver_set_buffer_length(int newBufferLength) {
	assert(driver_buffer_length <= PLATFORM_MAX_SOUND_BUFFER_LENGTH);
	if(newBufferLength > PLATFORM_MAX_SOUND_BUFFER_LENGTH)
		newBufferLength = PLATFORM_MAX_SOUND_BUFFER_LENGTH;
	driver_buffer_length = newBufferLength;
}

int sound_driver_get_gain() {
	return driver_gain;
}

void sound_driver_set_gain(int newGain) {
	if(newGain > 9)
		newGain = 9;
	driver_gain = newGain;
}

void sound_driver_write(int32_t* buf) {
	int16_t buf16[PLATFORM_MAX_SOUND_BUFFER_LENGTH * 2];
	int processed;
	int state;

	check();

	alGetSourcei(source[0], AL_SOURCE_STATE, &state);
	check();

	if(state == 0x1014) {
		alSourcePlay(source[0]);
	}

	alGetSourcei(source[0], AL_BUFFERS_PROCESSED, &processed);
	check();

	if(processed >= 1) {
		int c;
		ALuint buffer;
		const double gain = pow(10.0, (double)gain / 20.0);

		alListenerf(AL_GAIN, gain);
		alSourceUnqueueBuffers(source[0], 1, &buffer);
		check();

		for(c = 0; c < driver_buffer_length * 2; c++) {
			if(buf[c] < -32768)
				buf16[c] = -32768;
			else if(buf[c] > 32767)
				buf16[c] = 32767;
			else
				buf16[c] = buf[c];
		}

		alBufferData(buffer, AL_FORMAT_STEREO16, buf16, driver_buffer_length * 2 * 2, PLATFORM_FREQ);
		check();

		alSourceQueueBuffers(source[0], 1, &buffer);
		check();
	}
}

void sound_driver_write_cd(int16_t* buf) {
	int processed;
	int state;

	check();

	alGetSourcei(source[1], AL_SOURCE_STATE, &state);

	check();

	if(state == 0x1014) {
		alSourcePlay(source[1]);
	}
	alGetSourcei(source[1], AL_BUFFERS_PROCESSED, &processed);
	check();

	if(processed >= 1) {
		ALuint buffer;
		const double gain = pow(10.0, (double)gain / 20.0);
		alListenerf(AL_GAIN, gain);
		alSourceUnqueueBuffers(source[1], 1, &buffer);
		check();
		alBufferData(buffer, AL_FORMAT_STEREO16, buf, PLATFORM_CD_BUFFER_LENGTH * 2 * 2, PLATFORM_CD_FREQ);
		check();
		alSourceQueueBuffers(source[1], 1, &buffer);
		check();
	}
}
