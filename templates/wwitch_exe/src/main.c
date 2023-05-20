// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2023
#include <wonderful.h>
#include <sys/bios.h>

void main(int argc, char** argv) {
	text_screen_init();
	text_put_string((LCD_CHAR_WIDTH - 13) / 2, LCD_CHAR_HEIGHT / 2, "Hello, World!");
	while (key_wait() != KEY_START);
}
