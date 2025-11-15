#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "pico.h"
#include "pico/time.h"
#include "hardware/gpio.h"

#include "lcd.h"

#define TEXTBUF_SIZE_CHARS 256U
#define TEXTBUF_SIZE_BYTES TEXTBUF_SIZE_CHARS

#define LCD_DB4 6U
#define LCD_DB5 7U
#define LCD_DB6 8U
#define LCD_DB7 9U
#define LCD_RS 5U

#define LCD1_E 10U
#define LCD1_NCHARS 16U
#define LCD1_NLINES 2U

#define LCD2_E 11U
#define LCD2_NCHARS 20U
#define LCD2_NLINES 4U

__attribute__((aligned(32))) lcd_t lcd1 = {
	.db4 = LCD_DB4,
	.db5 = LCD_DB5,
	.db6 = LCD_DB6,
	.db7 = LCD_DB7,
	.rs = LCD_RS,
	.e = LCD1_E,
	.n_chars = LCD1_NCHARS,
	.n_lines = LCD1_NLINES
};

__attribute__((aligned(32))) lcd_t lcd2 = {
	.db4 = LCD_DB4,
	.db5 = LCD_DB5,
	.db6 = LCD_DB6,
	.db7 = LCD_DB7,
	.rs = LCD_RS,
	.e = LCD2_E,
	.n_chars = LCD2_NCHARS,
	.n_lines = LCD2_NLINES
};

__attribute__((aligned(32))) uint16_t num16 = 0u;
__attribute__((aligned(32))) char textbuf[TEXTBUF_SIZE_CHARS];

void main(void)
{
	memset(textbuf, 0, TEXTBUF_SIZE_BYTES);

	lcd_init(&lcd1);
	lcd_init(&lcd2);

	lcd_set_cursor_pos(&lcd1, 0u, 0u);
	lcd_print_text(&lcd1, "This is line 0");
	lcd_set_cursor_pos(&lcd1, 0u, 1u);
	lcd_print_text(&lcd1, "This is line 1");

	lcd_set_cursor_pos(&lcd2, 0u, 0u);
	lcd_print_text(&lcd2, "This is line 00");
	lcd_set_cursor_pos(&lcd2, 0u, 1u);
	lcd_print_text(&lcd2, "This is line 01");
	lcd_set_cursor_pos(&lcd2, 0u, 2u);
	lcd_print_text(&lcd2, "This is line 02");
	lcd_set_cursor_pos(&lcd2, 0u, 3u);
	lcd_print_text(&lcd2, "This is line 03");

	sleep_ms(4096);

	lcd_clear(&lcd1);
	lcd_clear(&lcd2);

	lcd_home(&lcd1);
	lcd_print_text(&lcd1, "Counting...");

	lcd_home(&lcd2);
	lcd_print_text(&lcd2, "Counting...");

_l_main_runtimeloop:

	snprintf(textbuf, TEXTBUF_SIZE_CHARS, "%u    ", num16);
	lcd_set_cursor_pos(&lcd2, 12u, 0u);
	lcd_print_text(&lcd2, textbuf);

	snprintf(textbuf, TEXTBUF_SIZE_CHARS, "%u  ", (num16 & 0xff));
	lcd_set_cursor_pos(&lcd1, 12u, 0u);
	lcd_print_text(&lcd1, textbuf);

	sleep_ms(512);
	num16++;

	goto _l_main_runtimeloop;
	return;
}

