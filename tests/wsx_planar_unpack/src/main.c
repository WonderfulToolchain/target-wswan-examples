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
	// Clear screen 1.
	outportb(IO_SCR_BASE, SCR1_BASE(0x1800));
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
	ws_display_set_shade_lut(SHADE_LUT_DEFAULT);
	outportw(0x20, 0x7520);

	// Unpack tiles.
	memset(MEM_TILE(0), 0, 16);
	for (int i = 0; i < 3; i++) {
		wsx_planar_unpack(MEM_TILE(i + 1), 8, smiley, WSX_PLANAR_UNPACK_1BPP_TO_2BPP_ZERO(i));
	}
	draw_smileys_on_screen(3);

	// Enable display.
	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);

	wait_for_keypress(KEY_A);

	outportw(IO_DISPLAY_CTRL, 0);

	for (int i = 0; i < 16; i++) {
		wsx_planar_unpack(MEM_TILE(i), 8, smiley, WSX_PLANAR_UNPACK_1BPP_TO_2BPP(i, 0));
	}
	draw_smileys_on_screen(15);

	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);

	wait_for_keypress(KEY_A);

	if (ws_system_is_color()) {
		outportw(IO_DISPLAY_CTRL, 0);
		ws_mode_set(WS_MODE_COLOR_4BPP);

		// Initialize 4BPP display mode.
		for (int i = 0; i < 16; i++) {
			MEM_COLOR_PALETTE(0)[i] = (15 - i) * 0x111;
		}

		// Unpack tiles.
		memset(MEM_TILE_4BPP(0), 0, 32);
		for (int i = 0; i < 15; i++) {
			wsx_planar_unpack(MEM_TILE_4BPP(i + 1), 8, smiley, WSX_PLANAR_UNPACK_1BPP_TO_4BPP_ZERO(i));
		}

		draw_smileys_on_screen(15);

		// Enable display.
		outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE);
	}

	while(1);
}
