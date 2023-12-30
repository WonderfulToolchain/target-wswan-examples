// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023
#include <string.h>
#include <wonderful.h>
#include <ws.h>
#include <ws/display.h>
#include <ws/hardware.h>
#include <ws/system.h>

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
	outportw(IO_DISPLAY_CTRL, 0);

	// Set the screen address.
	outportb(IO_SCR_BASE, SCR1_BASE(screen_1) | SCR2_BASE(screen_2));

	// Clear scroll registers.
	outportb(IO_SCR1_SCRL_X, 0);
	outportb(IO_SCR1_SCRL_Y, 0);
	outportb(IO_SCR2_SCRL_X, 0);
	outportb(IO_SCR2_SCRL_Y, 0);

	// If color mode is initialized, configure assets using color mode;
	// if not, configure them using mono mode.
#ifdef EXAMPLE_FORCE_MONO
	if (0) {
#else
	if (ws_system_mode_set(WS_MODE_COLOR_4BPP)) {
#endif
		// Clear tile 0.
		memset(MEM_TILE_4BPP_BANK0(0), 0, sizeof(ws_tile_4bpp_t));

		// Copy tiles.
		// DMA is Color-exclusive, but faster than memcpy().
		ws_dma_copy_words(MEM_TILE_4BPP_BANK0(1), gfx_color_pyramid_tiles, gfx_color_pyramid_tiles_size);
		ws_dma_copy_words(MEM_TILE_4BPP_BANK1(0), gfx_color_swan_tiles, gfx_color_swan_tiles_size);

		// Configure color palettes.
		ws_dma_copy_words(MEM_COLOR_PALETTE(0), gfx_color_pyramid_palette, gfx_color_pyramid_palette_size);
		ws_dma_copy_words(MEM_COLOR_PALETTE(12), gfx_color_swan_palette, gfx_color_swan_palette_size);

		// Write tilemaps.
		ws_screen_put_tiles(screen_1, gfx_color_pyramid_map, 0, 0, 28, 18);
		ws_screen_put_tiles(screen_2, gfx_color_swan_map, 28 - 16 - 2, 3, 16, 15);
	} else {
		// Clear tile 0.
		memset(MEM_TILE(0), 0, sizeof(ws_tile_t));

		// Clear screen 2.
		// On a "mono" model, only palettes 4-7 and 12-15 are translucent.
		ws_screen_fill_tiles(screen_2, SCR_ENTRY_PALETTE(12), 0, 0, 32, 32);

		// Copy tiles.
		memcpy(MEM_TILE(1), gfx_mono_pyramid_tiles, gfx_mono_pyramid_tiles_size);
		memcpy(MEM_TILE(384), gfx_mono_swan_tiles, gfx_mono_swan_tiles_size);

		// Configure mono palettes.
		ws_portcpy(IO_SCR_PAL_0, gfx_mono_pyramid_palette, gfx_mono_pyramid_palette_size);
		ws_portcpy(IO_SCR_PAL_12, gfx_mono_swan_palette, gfx_mono_swan_palette_size);

		// Configure mono shade LUT.
		ws_display_set_shade_lut(SHADE_LUT_DEFAULT);

		// Write tilemaps.
		ws_screen_put_tiles(screen_1, gfx_mono_pyramid_map, 0, 0, 28, 18);
		ws_screen_put_tiles(screen_2, gfx_mono_swan_map, 28 - 16 - 2, 3, 16, 15);
	}

	// Enable display: screen 1 and screen 2.
	outportw(IO_DISPLAY_CTRL, DISPLAY_SCR1_ENABLE | DISPLAY_SCR2_ENABLE);

	// Enable the VBlank interrupt.
	// Since this is the only interrupt enabled, "cpu_halt();" will only
	// wait for vertical blank.
	ws_hwint_set_default_handler_vblank();
	ws_hwint_enable(HWINT_VBLANK);

	// Enable CPU interrupts.
	cpu_irq_enable();

	// Wait indefinitely.
	while(1) {
		cpu_halt();

		// Move the second screen (with the swan) to the left by 1 pixel.
		outportb(IO_SCR2_SCRL_X, inportb(IO_SCR2_SCRL_X) + 1);
	}
}
