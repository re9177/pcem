#include <stdio.h>
#include <stdlib.h>
#include "ibm.h"
#include "devices/device.h" // REMOVE

#include "cdrom/cdrom-ioctl.h"
#include "cdrom/cdrom-image.h"
#include "ide/ide.h"

#include "filters.h"

#include "sound.h"
#include "devices/sound/sound_adlib.h"
#include "devices/sound/sound_adlibgold.h"
#include "devices/sound/sound_audiopci.h"
#include "devices/sound/sound_azt2316a.h"
#include "devices/sound/sound_pas16.h"
#include "devices/sound/sound_sb.h"
#include "devices/sound/sound_wss.h"
#include "devices/sound/sound_mmb.h"

#include "platform/sound.h"
#include "timer.h"

#include "devices/device.h"

extern SOUND_CARD* sound_cards[SOUND_MAX];

int sound_card_current = 0;
static int sound_card_last = 0;

SOUND_CARD sc_none = { "None", "none", NULL };
SOUND_CARD sc_adlib = { "Adlib", "adlib", &adlib_device };
SOUND_CARD sc_adlib_mca = { "Adlib", "adlib_mca", &adlib_mca_device };
SOUND_CARD sc_sb = { "Sound Blaster 1.0", "sb", &sb_1_device };
SOUND_CARD sc_sb1_5 = { "Sound Blaster 1.5", "sb1.5", &sb_15_device };
SOUND_CARD sc_sbmcv = { "Sound Blaster MCV", "sbmcv", &sb_mcv_device };
SOUND_CARD sc_sb2_0 = { "Sound Blaster 2.0", "sb2.0", &sb_2_device };
SOUND_CARD sc_sbprov1 = { "Sound Blaster Pro v1", "sbprov1", &sb_pro_v1_device };
SOUND_CARD sc_sbprov2 = { "Sound Blaster Pro v2", "sbprov2", &sb_pro_v2_device };
SOUND_CARD sc_sbpromcv = { "Sound Blaster Pro MCV", "sbpromcv", &sb_pro_mcv_device };
SOUND_CARD sc_sb16 = { "Sound Blaster 16", "sb16", &sb_16_device };
SOUND_CARD sc_sbawe32 = { "Sound Blaster AWE32", "sbawe32", &sb_awe32_device };
SOUND_CARD sc_adlibgold = { "Adlib Gold", "adlibgold", &adgold_device };
SOUND_CARD sc_wss = { "Windows Sound System", "wss", &wss_device };
SOUND_CARD sc_azt2316a = { "Aztech Sound Galaxy Pro 16 AB (Washington)", "azt2316a", &azt2316a_device };
SOUND_CARD sc_azt1605 = { "Aztech Sound Galaxy Nova 16 Extra (Clinton)", "azt1605", &azt1605_device };
SOUND_CARD sc_pas16 = { "Pro Audio Spectrum 16", "pas16", &pas16_device };
SOUND_CARD sc_es1371 = { "Ensoniq AudioPCI (ES1371)", "es1371", &es1371_device };
SOUND_CARD sc_sbpci128 = { "Sound Blaster PCI 128", "sbpci128", &es1371_device };
SOUND_CARD sc_mmb = { "Mindscape Music Board", "mmb", &mmb_device };

int sound_card_available(int card) {
	if(sound_cards[card] != NULL && sound_cards[card]->device != NULL)
		return device_available(sound_cards[card]->device);

	return 1;
}

char* sound_card_getname(int card) {
	if(sound_cards[card] != NULL)
		return sound_cards[card]->name;

	return "";
}

device_t* sound_card_getdevice(int card) {
	if(sound_cards[card] != NULL)
		return sound_cards[card]->device;

	return NULL;
}

int sound_card_has_config(int card) {
	if(sound_cards[card] == NULL || sound_cards[card]->device == NULL)
		return 0;
	return sound_cards[card]->device->config ? 1 : 0;
}

char* sound_card_get_internal_name(int card) {
	if(sound_cards[card] != NULL)
		return sound_cards[card]->internal_name;
	return "error-no-name";
}

int sound_card_get_from_internal_name(char* s) {
	int c = 0;

	while(sound_cards[c] != NULL) {
		if(!strcmp(sound_cards[c]->internal_name, s))
			return c;
		c++;
	}

	return 0;
}

void sound_card_init() {
	if(sound_cards[sound_card_current]->device)
		device_add(sound_cards[sound_card_current]->device);
	sound_card_last = sound_card_current;
}

static struct {
	void (*get_buffer)(int32_t* buffer, int len, void* p);
	void* priv;
} sound_handlers[8];

static int sound_handlers_num;

static pc_timer_t sound_poll_timer;
static uint64_t sound_poll_latch;
int sound_pos_global = 0;

int soundon = 1;

static int16_t cd_buffer[PLATFORM_CD_BUFFER_LENGTH * 2];
static uint32_t cd_vol_l, cd_vol_r;

int sound_buf_len = 48000 / 10;

static int32_t* outbuffer;

void sound_update_buf_length() {
	sound_driver_set_buffer_length((48000 / (1000 / sound_buf_len)) / sizeof(int));
}

void sound_set_cd_volume(unsigned int vol_l, unsigned int vol_r) {
	cd_vol_l = vol_l;
	cd_vol_r = vol_r;
}

void sound_init() {
	// Initialize sound driver.
	sound_driver_init();

	outbuffer = malloc(PLATFORM_MAX_SOUND_BUFFER_LENGTH * 2 * sizeof(int32_t));
}

void sound_shutdown() {
	// Shut down the driver first, as it may still be holding on to
	// our output buffer.
	sound_driver_shutdown();

	sound_handlers_num = 0;
	ioctl_audio_stop();
	image_audio_stop();

	free(outbuffer);
}

void sound_add_handler(void (*get_buffer)(int32_t* buffer, int len, void* p), void* p) {
	sound_handlers[sound_handlers_num].get_buffer = get_buffer;
	sound_handlers[sound_handlers_num].priv = p;
	sound_handlers_num++;
}

static int cd_pos = 0;
void sound_poll(void* priv) {
	timer_advance_u64(&sound_poll_timer, sound_poll_latch);

	cd_pos++;
	if(cd_pos == (PLATFORM_CD_BUFFER_LENGTH * PLATFORM_FREQ) / PLATFORM_CD_FREQ) {
		cd_pos = 0;

		// NOTE(lily): Just going to put this here.
		// I don't think it needs to be running on a different thread,
		// and if it does run better on a thread,
		// perhaps audio mixing as a whole would be better running on another thread.

		memset(cd_buffer, 0, PLATFORM_CD_BUFFER_LENGTH * 2 * 2);
		ioctl_audio_callback(cd_buffer, PLATFORM_CD_BUFFER_LENGTH * 2);
		image_audio_callback(cd_buffer, PLATFORM_CD_BUFFER_LENGTH * 2);
		if(soundon) {
			int32_t atapi_vol_l = atapi_get_cd_volume(0);
			int32_t atapi_vol_r = atapi_get_cd_volume(1);
			int channel_select[2];

			channel_select[0] = atapi_get_cd_channel(0);
			channel_select[1] = atapi_get_cd_channel(1);

			for(int c = 0; c < PLATFORM_CD_BUFFER_LENGTH * 2; c += 2) {
				int32_t cd_buffer_temp[2] = { 0, 0 };

				/*First, adjust input from drive according to ATAPI volume.*/
				cd_buffer[c] = ((int32_t)cd_buffer[c] * atapi_vol_l) / 255;
				cd_buffer[c + 1] = ((int32_t)cd_buffer[c + 1] * atapi_vol_r) / 255;

				/*Apply ATAPI channel select*/
				if(channel_select[0] & 1)
					cd_buffer_temp[0] += cd_buffer[c];
				if(channel_select[0] & 2)
					cd_buffer_temp[1] += cd_buffer[c];
				if(channel_select[1] & 1)
					cd_buffer_temp[0] += cd_buffer[c + 1];
				if(channel_select[1] & 2)
					cd_buffer_temp[1] += cd_buffer[c + 1];

				/*Apply sound card CD volume*/
				cd_buffer_temp[0] = (cd_buffer_temp[0] * (int)cd_vol_l) / 65535;
				cd_buffer_temp[1] = (cd_buffer_temp[1] * (int)cd_vol_r) / 65535;

				if(cd_buffer_temp[0] > 32767)
					cd_buffer_temp[0] = 32767;
				if(cd_buffer_temp[0] < -32768)
					cd_buffer_temp[0] = -32768;
				if(cd_buffer_temp[1] > 32767)
					cd_buffer_temp[1] = 32767;
				if(cd_buffer_temp[1] < -32768)
					cd_buffer_temp[1] = -32768;

				cd_buffer[c] = cd_buffer_temp[0];
				cd_buffer[c + 1] = cd_buffer_temp[1];
			}

			sound_driver_write_cd(cd_buffer);
		}
	}

	sound_pos_global++;
	const int buffer_length = sound_driver_get_buffer_length();
	if(sound_pos_global == buffer_length) {
		memset(outbuffer, 0, buffer_length * 2 * sizeof(int32_t));

		for(int c = 0; c < sound_handlers_num; c++)
			sound_handlers[c].get_buffer(outbuffer, buffer_length, sound_handlers[c].priv);

		if(soundon)
			sound_driver_write(outbuffer);

		sound_pos_global = 0;
		sound_update_buf_length();
	}
}

void sound_speed_changed() {
	sound_poll_latch = (uint64_t)((double)TIMER_USEC * (1000000.0 / (double)(PLATFORM_FREQ)));
}

void sound_reset() {
	timer_add(&sound_poll_timer, sound_poll, NULL, 1);

	sound_handlers_num = 0;

	sound_set_cd_volume(65535, 65535);
	ioctl_audio_stop();
	image_audio_stop();
}

void sound_init_builtin() {
	pcem_add_sound(&sc_none);
	pcem_add_sound(&sc_adlib);
	pcem_add_sound(&sc_adlib_mca);
	pcem_add_sound(&sc_sb);
	pcem_add_sound(&sc_sb1_5);
	pcem_add_sound(&sc_sbmcv);
	pcem_add_sound(&sc_sb2_0);
	pcem_add_sound(&sc_sbprov1);
	pcem_add_sound(&sc_sbprov2);
	pcem_add_sound(&sc_sbpromcv);
	pcem_add_sound(&sc_sb16);
	pcem_add_sound(&sc_sbawe32);
	pcem_add_sound(&sc_adlibgold);
	pcem_add_sound(&sc_wss);
	pcem_add_sound(&sc_azt2316a);
	pcem_add_sound(&sc_azt1605);
	pcem_add_sound(&sc_pas16);
	pcem_add_sound(&sc_es1371);
	pcem_add_sound(&sc_sbpci128);
	pcem_add_sound(&sc_mmb);
}
