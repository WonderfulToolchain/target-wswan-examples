// SPDX-License-Identifier: CC0-1.0
//
// SPDX-FileContributor: Adrian "asie" Siekierka, 2024

#include <string.h>
#include <wonderful.h>
#include <ws.h>
#include <wwcl.h>

WWCL_INIT_MODE_JAPANESE1();

static const char __far text_mode_str0[] = "TEXT_MODE_ANK";
static const char __far text_mode_str1[] = "TEXT_MODE_ANK_SJIS";
static const char __far text_mode_str2[] = "TEXT_MODE_SJIS";
static const char __far* __far text_mode_str[] = {
	text_mode_str0,
	text_mode_str1,
	text_mode_str2
};

void main(void) {
	int curr_mode = 0;

	wwcl_init();

	while(1) {
		text_set_mode(curr_mode);
		text_screen_init();
		text_put_string(
			(28 - strlen(text_mode_str[curr_mode])) >> 1,
			1,
			text_mode_str[curr_mode]
		);

		for (uint16_t ch = 32; ch < 127; ch++) {
			text_put_char(
				((28 - 16) >> 1) + (ch & 0xF),
				((18 - 6) >> 1) + (ch >> 4) - 2,
				ch
			);
		}

		while (key_hit_check() != KEY_A);
		curr_mode = (curr_mode + 1) % 3;
	}
}

