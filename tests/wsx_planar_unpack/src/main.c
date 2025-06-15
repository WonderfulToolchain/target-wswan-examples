// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023
#include <string.h>
#include <wonderful.h>
#include <ws.h>
#include <wsx/planar_unpack.h>
#include "smiley_bin.h"

#define SCREEN1 ((uint16_t*) 0x1800)

static void draw_smileys_on_screen(int count) {
	ws_display_set_screen1_address(0x1800);

	// Clear screen 1.
	memset(SCREEN1, 0, 0x800);

	// Set tiles on screen.
	for (int i = 0; i < count; i++) {
		SCREEN1[i] = i + 1;
	}
}

static void wait_for_keypress(uint16_t mask) {
	while(ws_keypad_scan() != mask);
	while(ws_keypad_scan() != 0);
}

void main(void) {
	// Initialize display palette.
	ws_display_set_shade_lut(WS_DISPLAY_SHADE_LUT_DEFAULT);
	outportw(WS_SCR_PAL_0_PORT, 0x7520);

	// Unpack tiles.
	memset(WS_TILE_MEM(0), 0, 16);
	for (int i = 0; i < 3; i++) {
		wsx_planar_unpack(WS_TILE_MEM(i + 1), 8, smiley, WSX_PLANAR_UNPACK_1BPP_TO_2BPP_ZERO(i));
	}
	draw_smileys_on_screen(3);

	// Enable display.
	outportw(WS_DISPLAY_CTRL_PORT, WS_DISPLAY_CTRL_SCR1_ENABLE);

	wait_for_keypress(WS_KEY_A);

	outportw(WS_DISPLAY_CTRL_PORT, 0);

	for (int i = 0; i < 16; i++) {
		wsx_planar_unpack(WS_TILE_MEM(i), 8, smiley, WSX_PLANAR_UNPACK_1BPP_TO_2BPP((i >> 2), (i & 3)));
	}
	draw_smileys_on_screen(15);

	outportw(WS_DISPLAY_CTRL_PORT, WS_DISPLAY_CTRL_SCR1_ENABLE);

	wait_for_keypress(WS_KEY_A);

	if (ws_system_is_color_model()) {
		outportw(WS_DISPLAY_CTRL_PORT, 0);
		ws_system_set_mode(WS_MODE_COLOR_4BPP);

		// Initialize 4BPP display mode.
		for (int i = 0; i < 16; i++) {
			WS_DISPLAY_COLOR_MEM(0)[i] = (15 - i) * 0x111;
		}

		// Unpack tiles.
		memset(WS_TILE_4BPP_MEM(0), 0, 32);
		for (int i = 0; i < 15; i++) {
			wsx_planar_unpack(WS_TILE_4BPP_MEM(i + 1), 8, smiley, WSX_PLANAR_UNPACK_1BPP_TO_4BPP_ZERO(i));
		}

		draw_smileys_on_screen(15);

		// Enable display.
		outportw(WS_DISPLAY_CTRL_PORT, WS_DISPLAY_CTRL_SCR1_ENABLE);
	}

	while(1);
}
