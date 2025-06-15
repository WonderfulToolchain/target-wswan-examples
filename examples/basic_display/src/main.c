// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023
#include <string.h>
#include <wonderful.h>
#include <ws.h>

// Assets
#include "color/pyramid.h"
#include "color/swan.h"
#include "mono/pyramid.h"
#include "mono/swan.h"

// Internal RAM global variables.
//
// The IRAM_IMPLEMENTATION should be defined in exactly one file, and precede
// the iram.h include there. You can only provide the globals in one .c file
// at a time - the other files can only reference them.
#define IRAM_IMPLEMENTATION
#include "iram.h"

// Uncomment this to test the "mono" mode in emulators which do not support
// forcing it, or on a real "color" device.
// #define EXAMPLE_FORCE_MONO

void main(void) {
	// Disable display.
	outportw(WS_DISPLAY_CTRL_PORT, 0);

	// Set the screen address.
	ws_display_set_screen_addresses(screen_1, screen_2);

	// Clear scroll registers.
	ws_display_scroll_screen1_to(0, 0);
	ws_display_scroll_screen2_to(0, 0);

	// If color mode is initialized, configure assets using color mode;
	// if not, configure them using mono mode.
#ifdef EXAMPLE_FORCE_MONO
	if (0) {
#else
	if (ws_system_set_mode(WS_MODE_COLOR_4BPP)) {
#endif
		// Clear tile 0.
		memset(WS_TILE_4BPP_MEM(0), 0, sizeof(ws_display_tile_4bpp_t));

		// Copy tiles.
		// DMA is Color-exclusive, but faster than memcpy().
		ws_gdma_copy(WS_TILE_4BPP_BANKED_MEM(0, 1), gfx_color_pyramid_tiles, gfx_color_pyramid_tiles_size);
		ws_gdma_copy(WS_TILE_4BPP_BANKED_MEM(1, 0), gfx_color_swan_tiles, gfx_color_swan_tiles_size);

		// Configure color palettes.
		ws_gdma_copy(WS_SCREEN_COLOR_MEM(0), gfx_color_pyramid_palette, gfx_color_pyramid_palette_size);
		ws_gdma_copy(WS_SCREEN_COLOR_MEM(12), gfx_color_swan_palette, gfx_color_swan_palette_size);

		// Write tilemaps.
		ws_screen_put_tiles(screen_1, gfx_color_pyramid_map, 0, 0, 28, 18);
		ws_screen_put_tiles(screen_2, gfx_color_swan_map, 28 - 16 - 2, 3, 16, 15);
	} else {
		// Clear tile 0.
		memset(WS_TILE_MEM(0), 0, sizeof(ws_display_tile_t));

		// Clear screen 2.
		// On a "mono" model, only palettes 4-7 and 12-15 are translucent.
		ws_screen_fill_tiles(screen_2, WS_SCREEN_ATTR_PALETTE(12), 0, 0, 32, 32);

		// Copy tiles.
		memcpy(WS_TILE_MEM(1), gfx_mono_pyramid_tiles, gfx_mono_pyramid_tiles_size);
		memcpy(WS_TILE_MEM(384), gfx_mono_swan_tiles, gfx_mono_swan_tiles_size);

		// Configure mono palettes.
		ws_portcpy(WS_SCR_PAL_0_PORT, gfx_mono_pyramid_palette, gfx_mono_pyramid_palette_size);
		ws_portcpy(WS_SCR_PAL_12_PORT, gfx_mono_swan_palette, gfx_mono_swan_palette_size);

		// Configure mono shade LUT.
		ws_display_set_shade_lut(WS_DISPLAY_SHADE_LUT_DEFAULT);

		// Write tilemaps.
		ws_screen_put_tiles(screen_1, gfx_mono_pyramid_map, 0, 0, 28, 18);
		ws_screen_put_tiles(screen_2, gfx_mono_swan_map, 28 - 16 - 2, 3, 16, 15);
	}

	// Enable display: screen 1 and screen 2.
	outportw(WS_DISPLAY_CTRL_PORT, WS_DISPLAY_CTRL_SCR1_ENABLE | WS_DISPLAY_CTRL_SCR2_ENABLE);

	// Enable the VBlank interrupt.
	// Since this is the only interrupt enabled, "ia16_halt();" will only
	// wait for vertical blank.
	ws_int_set_default_handler_vblank();
	ws_int_enable(WS_INT_ENABLE_VBLANK);

	// Enable CPU interrupts.
	ia16_enable_irq();

	// Wait indefinitely.
	while(1) {
		ia16_halt();

		// Move the second screen (with the swan) to the left by 1 pixel.
		ws_display_scroll_screen2_by(1, 0);
	}
}
