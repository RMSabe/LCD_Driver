/*
 * Generic Alphanumeric LCD display driver for Raspberry Pi Pico
 * Version 1.1
 *
 * Author: Rafael Sabe
 * Email: rafaelmsabe@gmail.com
 */

#ifndef LCD_H
#define LCD_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define __LCD_STATUS_ERROR -1
#define __LCD_STATUS_UNINITIALIZED 0
#define __LCD_STATUS_INITIALIZED 1

#define LCD_DISPLAYMODE_DISPLAY_OFF 0
#define LCD_DISPLAYMODE_DISPLAY_ON_CURSOR_OFF 1
#define LCD_DISPLAYMODE_DISPLAY_ON_CURSOR_ON 2
#define LCD_DISPLAYMODE_DISPLAY_ON_CURSOR_BLINK 3

struct _lcd {
	uint8_t db4;		/*DB4 GPIO PIN*/
	uint8_t db5;		/*DB5 GPIO PIN*/
	uint8_t db6;		/*DB6 GPIO PIN*/
	uint8_t db7;		/*DB7 GPIO PIN*/
	uint8_t rs;		/*RS GPIO PIN*/
	uint8_t e;		/*E GPIO PIN*/
	uint8_t n_chars;	/*NUMBER CHARACTERS PER LINE*/
	uint8_t n_lines;	/*NUMBER LINES*/
	intptr_t _status;	/*IGNORE (INTERNAL USE)*/
};

typedef struct _lcd lcd_t;

/*
 * lcd_init()
 * initializes LCD object. Must be called before calling any other functions in this driver.
 *
 * returns true if successful, false otherwise.
 */

extern bool lcd_init(lcd_t *p_lcd);

/*
 * lcd_clear()
 * clear the LCD screen.
 *
 * returns true if successful, false otherwise.
 */

extern bool lcd_clear(const lcd_t *p_lcd);

/*
 * lcd_home()
 * reset cursor position to 0, 0
 *
 * returns true if successful, false otherwise.
 */

extern bool lcd_home(const lcd_t *p_lcd);

/*
 * lcd_set_display_mode()
 * sets the display power and cursor show modes
 *
 * returns true if successful, false otherwise.
 */

extern bool lcd_set_display_mode(const lcd_t *p_lcd, intptr_t display_mode);

/*
 * lcd_set_cursor_pos()
 * sets the display cursor position.
 *
 * returns true if successful, false otherwise.
 */

extern bool lcd_set_cursor_pos(const lcd_t *p_lcd, uint8_t cx, uint8_t cy);

/*
 * lcd_print_char()
 * prints a single character at the current cursor position
 *
 * returns true if successful, false otherwise.
 */

extern bool lcd_print_char(const lcd_t *p_lcd, char c);

/*
 * lcd_print_text()
 * prints a null-terminated string at the current cursor position
 *
 * returns true if successful, false otherwise.
 */

extern bool lcd_print_text(const lcd_t *p_lcd, const char *text);

/*
 * lcd_print_text_deflen()
 * prints a string of length "len" at the current cursor position
 *
 * returns true if successful, false otherwise.
 */

extern bool lcd_print_text_deflen(const lcd_t *p_lcd, const char *text, uintptr_t len);

/*
 * lcd_fill_screen_char()
 * print a given character repeatedly filling the whole screen.
 *
 * returns true if successful, false otherwise.
 */

extern bool lcd_fill_screen_char(const lcd_t *p_lcd, char c);

#endif /*LCD_H*/

