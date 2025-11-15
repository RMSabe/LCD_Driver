/*
 * Generic Alphanumeric LCD display driver for Raspberry Pi Pico
 * Version 1.1
 *
 * Author: Rafael Sabe
 * Email: rafaelmsabe@gmail.com
 */

#include "lcd.h"

#include "pico.h"
#include "pico/time.h"
#include "hardware/gpio.h"

#define __LCD_EN_DELAY_US 1U
#define __LCD_CMD_DELAY_US 1024U

extern void _lcd_send_byte(const lcd_t *p_lcd, bool reg, uint8_t byte);
extern void _lcd_write_nibble(const lcd_t *p_lcd, uint8_t nibble);
extern void _lcd_send_init_nibble(const lcd_t *p_lcd);
extern bool _lcd_validate_info(const lcd_t *p_lcd);
extern bool _phys_text_cx_cy_to_virt_text_cx_cy(const lcd_t *p_lcd, uint8_t *p_virtcx, uint8_t *p_virtcy, uint8_t physcx, uint8_t physcy);

bool lcd_init(lcd_t *p_lcd)
{
	if(p_lcd == NULL) return false;

	if(!_lcd_validate_info(p_lcd))
	{
		p_lcd->_status = __LCD_STATUS_ERROR;
		return false;
	}

	gpio_init(p_lcd->e);
	gpio_set_dir(p_lcd->e, GPIO_OUT);
	gpio_put(p_lcd->e, 0);

	gpio_init(p_lcd->rs);
	gpio_set_dir(p_lcd->rs, GPIO_OUT);

	gpio_init(p_lcd->db4);
	gpio_init(p_lcd->db5);
	gpio_init(p_lcd->db6);
	gpio_init(p_lcd->db7);

	gpio_set_dir(p_lcd->db4, GPIO_OUT);
	gpio_set_dir(p_lcd->db5, GPIO_OUT);
	gpio_set_dir(p_lcd->db6, GPIO_OUT);
	gpio_set_dir(p_lcd->db7, GPIO_OUT);

	_lcd_send_init_nibble(p_lcd);

	/*Default Settings*/
	_lcd_send_byte(p_lcd, false, 0x28);
	_lcd_send_byte(p_lcd, false, 0x01);
	_lcd_send_byte(p_lcd, false, 0x80);
	_lcd_send_byte(p_lcd, false, 0x0c);

	p_lcd->_status = __LCD_STATUS_INITIALIZED;
	return true;
}

bool lcd_clear(const lcd_t *p_lcd)
{
	if(p_lcd == NULL) return false;
	if(p_lcd->_status != __LCD_STATUS_INITIALIZED) return false;

	_lcd_send_byte(p_lcd, false, 0x01);
	return true;
}

bool lcd_home(const lcd_t *p_lcd)
{
	if(p_lcd == NULL) return false;
	if(p_lcd->_status != __LCD_STATUS_INITIALIZED) return false;

	_lcd_send_byte(p_lcd, false, 0x02);
	return true;
}

bool lcd_set_display_mode(const lcd_t *p_lcd, intptr_t display_mode)
{
	if(p_lcd == NULL) return false;
	if(p_lcd->_status != __LCD_STATUS_INITIALIZED) return false;

	switch(display_mode)
	{
		case LCD_DISPLAYMODE_DISPLAY_OFF:
			_lcd_send_byte(p_lcd, false, 0x08);
			return true;

		case LCD_DISPLAYMODE_DISPLAY_ON_CURSOR_OFF:
			_lcd_send_byte(p_lcd, false, 0x0c);
			return true;

		case LCD_DISPLAYMODE_DISPLAY_ON_CURSOR_ON:
			_lcd_send_byte(p_lcd, false, 0x0e);
			return true;

		case LCD_DISPLAYMODE_DISPLAY_ON_CURSOR_BLINK:
			_lcd_send_byte(p_lcd, false, 0x0f);
			return true;
	}

	return false;
}

bool lcd_set_cursor_pos(const lcd_t *p_lcd, uint8_t cx, uint8_t cy)
{
	uint8_t num8;

	if(p_lcd == NULL) return false;
	if(p_lcd->_status != __LCD_STATUS_INITIALIZED) return false;

	if(!_phys_text_cx_cy_to_virt_text_cx_cy(p_lcd, &cx, &cy, cx, cy)) return false;

	if(cy) _lcd_send_byte(p_lcd, false, 0xc0);
	else _lcd_send_byte(p_lcd, false, 0x80);

	num8 = 0u;
	while(num8 < cx)
	{
		_lcd_send_byte(p_lcd, false, 0x14);
		num8++;
	}

	return true;
}

bool lcd_print_char(const lcd_t *p_lcd, char c)
{
	if(p_lcd == NULL) return false;
	if(p_lcd->_status != __LCD_STATUS_INITIALIZED) return false;

	_lcd_send_byte(p_lcd, true, (uint8_t) c);
	return true;
}

bool lcd_print_text(const lcd_t *p_lcd, const char *text)
{
	uintptr_t len;

	if(p_lcd == NULL) return false;
	if(p_lcd->_status != __LCD_STATUS_INITIALIZED) return false;
	if(text == NULL) return false;

	len = 0u;
	while(text[len] != '\0') len++;

	lcd_print_text_deflen(p_lcd, text, len);

	return true;
}

bool lcd_print_text_deflen(const lcd_t *p_lcd, const char *text, uintptr_t len)
{
	uintptr_t n_char;

	if(p_lcd == NULL) return false;
	if(p_lcd->_status != __LCD_STATUS_INITIALIZED) return false;
	if(text == NULL) return false;

	n_char = 0u;
	while(n_char < len)
	{
		_lcd_send_byte(p_lcd, true, (uint8_t) text[n_char]);
		n_char++;
	}

	return true;
}

bool lcd_fill_screen_char(const lcd_t *p_lcd, char c)
{
	uint8_t n_chars;
	uint8_t n_lines;

	uint8_t n_char;
	uint8_t n_line;

	if(p_lcd == NULL) return false;
	if(p_lcd->_status != __LCD_STATUS_INITIALIZED) return false;

	n_chars = p_lcd->n_chars;
	n_lines = p_lcd->n_lines;

	for(n_line = 0u; n_line < n_lines; n_line++)
	{
		lcd_set_cursor_pos(p_lcd, 0u, n_line);

		for(n_char = 0u; n_char < n_chars; n_char++)
			lcd_print_char(p_lcd, c);
	}

	return true;
}

void _lcd_send_byte(const lcd_t *p_lcd, bool reg, uint8_t byte)
{
	gpio_put(p_lcd->e, 0);
	gpio_put(p_lcd->rs, reg);
	sleep_us(__LCD_EN_DELAY_US);

	_lcd_write_nibble(p_lcd, (byte >> 4));
	gpio_put(p_lcd->e, 1);
	sleep_us(__LCD_EN_DELAY_US);
	gpio_put(p_lcd->e, 0);
	sleep_us(__LCD_EN_DELAY_US);

	_lcd_write_nibble(p_lcd, (byte & 0xf));
	gpio_put(p_lcd->e, 1);
	sleep_us(__LCD_EN_DELAY_US);
	gpio_put(p_lcd->e, 0);
	sleep_us(__LCD_CMD_DELAY_US);

	return;
}

void _lcd_write_nibble(const lcd_t *p_lcd, uint8_t nibble)
{
	gpio_put(p_lcd->db7, (nibble & 0x8));
	gpio_put(p_lcd->db6, (nibble & 0x4));
	gpio_put(p_lcd->db5, (nibble & 0x2));
	gpio_put(p_lcd->db4, (nibble & 0x1));

	return;
}

void _lcd_send_init_nibble(const lcd_t *p_lcd)
{
	gpio_put(p_lcd->e, 0);
	gpio_put(p_lcd->rs, 0);
	sleep_us(__LCD_EN_DELAY_US);

	_lcd_write_nibble(p_lcd, 0x2);
	gpio_put(p_lcd->e, 1);
	sleep_us(__LCD_EN_DELAY_US);
	gpio_put(p_lcd->e, 0);
	sleep_us((__LCD_CMD_DELAY_US << 1));

	return;
}

bool _lcd_validate_info(const lcd_t *p_lcd)
{
	uintptr_t n_byte;

	for(n_byte = 0u; n_byte < 8u; n_byte++) if(((const uint8_t*) p_lcd)[n_byte] == 0xff) return false;

	if(!p_lcd->n_chars) return false;
	if(!p_lcd->n_lines) return false;

	return true;
}

bool _phys_text_cx_cy_to_virt_text_cx_cy(const lcd_t *p_lcd, uint8_t *p_virtcx, uint8_t *p_virtcy, uint8_t physcx, uint8_t physcy)
{
	uint8_t virtcx;
	uint8_t virtcy;

	if(physcx >= p_lcd->n_chars) return false;
	if(physcy >= p_lcd->n_lines) return false;

	virtcy = (physcy & 0x1);

	virtcx = physcx;
	physcy = (physcy >> 1);

	virtcx += physcy*(p_lcd->n_chars);

	if(p_virtcx != NULL) *p_virtcx = virtcx;
	if(p_virtcy != NULL) *p_virtcy = virtcy;

	return true;
}

