// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2025
#include <stdio.h>
#include <wonderful.h>
#include <ws.h>
#include <wse/memory.h>
#include <wsx/console.h>

WSE_RESERVE_TILES(512, 0);

const char __far hello[] = "Hello, world!";

void main(void) {
	wsx_console_init_default(&wse_screen1);
	printf(hello);
	ws_display_set_control(WS_DISPLAY_CTRL_SCR1_ENABLE);

	while(1) {
		ia16_halt();
	}
}
