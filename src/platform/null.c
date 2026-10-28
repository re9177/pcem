// Null platform implementation.
#include "plat-joystick.h"
#include "plat-keyboard.h"
#include "plat-mouse.h"
#include <string.h>

int mouse_buttons;
static int mouse_x = 0, mouse_y = 0, mouse_z = 0;

int mouse[3];

void mouse_init() {
	memset(mouse, 0, sizeof(mouse));
}

void mouse_close() {
}

void mouse_wheel_update(int dir) {
	mouse[2] += dir;
}

void mouse_poll_host() {
	mouse_x = mouse_y = mouse_z = mouse_buttons = 0;
}

void mouse_get_mickeys(int* x, int* y, int* z) {
	*x = mouse_x;
	*y = mouse_y;
	*z = mouse_z;
	mouse_x = mouse_y = mouse_z = 0;
}

uint8_t pcem_key[272];
int rawinputkey[272];

void keyboard_init() {
	memset(pcem_key, 0, sizeof(pcem_key));
}

void keyboard_close() {
}

void keyboard_poll_host() {

}


int joysticks_present;
joystick_t joystick_state[MAX_JOYSTICKS];

plat_joystick_t plat_joystick_state[MAX_PLAT_JOYSTICKS];

void joystick_init() {
	joysticks_present = 0;
}

void joystick_close() {

}

static int joystick_get_axis(int joystick_nr, int mapping) {
	return 0;
}
void joystick_poll() {

}
