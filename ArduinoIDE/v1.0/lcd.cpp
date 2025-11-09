/*
 * Generic Alphanumeric LCD Display Driver for Arduino IDE.
 * Version 1.0
 *
 * Author: Rafael Sabe
 * Email: rafaelmsabe@gmail.com
 */

#include "lcd.hpp"

LCD::LCD(uint8_t db4, uint8_t db5, uint8_t db6, uint8_t db7, uint8_t rs, uint8_t e, uint8_t nCharsPerLine, uint8_t nLines)
{
	this->resetPinout(db4, db5, db6, db7, rs, e);
	this->resetDisplaySize(nCharsPerLine, nLines);
}

LCD::~LCD(void)
{
}

bool LCD::begin(void)
{
	if(this->_status > 0) return true;

	this->_status = this->STATUS_UNINITIALIZED;

	if(!this->_validate_info())
	{
		this->_status = this->STATUS_ERROR;
		return false;
	}

	pinMode(this->_info.e, OUTPUT);
	digitalWrite(this->_info.e, 0);

	pinMode(this->_info.rs, OUTPUT);
	pinMode(this->_info.db4, OUTPUT);
	pinMode(this->_info.db5, OUTPUT);
	pinMode(this->_info.db6, OUTPUT);
	pinMode(this->_info.db7, OUTPUT);

	this->_send_init_nibble();

	/*Default initialization settings*/
	this->_send_byte(false, 0x28);
	this->_send_byte(false, 0x01);
	this->_send_byte(false, 0x80);
	this->_send_byte(false, 0x0c);

	this->_status = this->STATUS_INITIALIZED;
	return true;
}

void LCD::resetPinout(uint8_t db4, uint8_t db5, uint8_t db6, uint8_t db7, uint8_t rs, uint8_t e)
{
	this->_status = this->STATUS_UNINITIALIZED;

	this->_info.db4 = db4;
	this->_info.db5 = db5;
	this->_info.db6 = db6;
	this->_info.db7 = db7;
	this->_info.rs = rs;
	this->_info.e = e;

	return;
}

void LCD::resetDisplaySize(uint8_t nCharsPerLine, uint8_t nLines)
{
	this->_status = this->STATUS_UNINITIALIZED;

	this->_info.n_chars = nCharsPerLine;
	this->_info.n_lines = nLines;

	return;
}

intptr_t LCD::getStatus(void)
{
	return this->_status;
}

int8_t LCD::getNCharsPerLine(void)
{
	if(this->_status < 1) return -1;

	return (int8_t) this->_info.n_chars;
}

int8_t LCD::getNLines(void)
{
	if(this->_status < 1) return -1;

	return (int8_t) this->_info.n_lines;
}

bool LCD::setDisplayMode(intptr_t displayMode)
{
	if(this->_status < 1) return false;

	switch(displayMode)
	{
		case this->DISPLAYMODE_DISPLAY_OFF:
			this->_send_byte(false, 0x08);
			break;

		case this->DISPLAYMODE_DISPLAY_ON_CURSOR_OFF:
			this->_send_byte(false, 0x0c);
			break;

		case this->DISPLAYMODE_DISPLAY_ON_CURSOR_ON:
			this->_send_byte(false, 0x0e);
			break;

		case this->DISPLAYMODE_DISPLAY_ON_CURSOR_BLINK:
			this->_send_byte(false, 0x0f);
			break;
	}

	return true;
}

bool LCD::clear(void)
{
	if(this->_status < 1) return false;

	this->_send_byte(false, 0x01);
	return true;
}

bool LCD::home(void)
{
	if(this->_status < 1) return false;

	this->_send_byte(false, 0x02);
	return true;
}

bool LCD::setCursorPosition(uint8_t cx, uint8_t cy)
{
	uint8_t num8 = 0u;

	if(this->_status < 1) return false;

	if(!this->_phys_text_cx_cy_to_virt_text_cx_cy(&cx, &cy, cx, cy)) return false;

	if(cy) this->_send_byte(false, 0xc0);
	else this->_send_byte(false, 0x80);

	num8 = 0u;
	while(num8 < cx)
	{
		this->_send_byte(false, 0x14);
		num8++;
	}

	return true;
}

bool LCD::printChar(char c)
{
	if(this->_status < 1) return false;

	this->_send_byte(true, (uint8_t) c);
	return true;
}

bool LCD::printText(const char *text)
{
	uintptr_t length = 0u;

	if(this->_status < 1) return false;
	if(text == NULL) return false;

	while(text[length] != '\0') length++;

	return this->printText(text, length);
}

bool LCD::printText(const char *text, uintptr_t length)
{
	uintptr_t n_char = 0u;

	if(this->_status < 1) return false;
	if(text == NULL) return false;

	n_char = 0u;
	while(n_char < length)
	{
		this->_send_byte(true, (uint8_t) text[n_char]);
		n_char++;
	}

	return true;
}

bool LCD::fillScreenChar(char c)
{
	uint8_t n_char = 0u;
	uint8_t n_line = 0u;
	uint8_t n_chars = 0u;
	uint8_t n_lines = 0u;

	if(this->_status < 1) return false;

	n_chars = this->_info.n_chars;
	n_lines = this->_info.n_lines;

	for(n_line = 0u; n_line < n_lines; n_line++)
	{
		this->setCursorPosition(0u, n_line);

		for(n_char = 0u; n_char < n_chars; n_char++)
			this->_send_byte(true, (uint8_t) c);
	}

	return true;
}

void LCD::_send_byte(bool reg, uint8_t byte)
{
	digitalWrite(this->_info.e, 0);
	digitalWrite(this->_info.rs, reg);

	delayMicroseconds(this->_EN_DELAY_US);

	this->_write_nibble(byte >> 4);
	digitalWrite(this->_info.e, 1);
	delayMicroseconds(this->_EN_DELAY_US);

	digitalWrite(this->_info.e, 0);
	delayMicroseconds(this->_EN_DELAY_US);

	this->_write_nibble(byte & 0xf);
	digitalWrite(this->_info.e, 1);
	delayMicroseconds(this->_EN_DELAY_US);

	digitalWrite(this->_info.e, 0);
	delayMicroseconds(this->_CMD_DELAY_US);

	return;
}

void LCD::_write_nibble(uint8_t nibble)
{
	digitalWrite(this->_info.db7, (nibble & 0x8));
	digitalWrite(this->_info.db6, (nibble & 0x4));
	digitalWrite(this->_info.db5, (nibble & 0x2));
	digitalWrite(this->_info.db4, (nibble & 0x1));

	return;
}

void LCD::_send_init_nibble(void)
{
	digitalWrite(this->_info.e, 0);
	digitalWrite(this->_info.rs, 0);

	delayMicroseconds(this->_EN_DELAY_US);

	this->_write_nibble(0x2);
	digitalWrite(this->_info.e, 1);
	delayMicroseconds(this->_EN_DELAY_US);

	digitalWrite(this->_info.e, 0);
	delayMicroseconds((this->_CMD_DELAY_US) << 1);

	return;
}

bool LCD::_validate_info(void)
{
	uintptr_t n_byte = 0u;
	uint8_t *p_info = NULL;

	p_info = (uint8_t*) &(this->_info);

	for(n_byte = 0u; n_byte < sizeof(struct _lcd_info); n_byte++) if(p_info[n_byte] == 0xff) return false;

	if(!this->_info.n_chars) return false;
	if(!this->_info.n_lines) return false;

	return true;
}

bool LCD::_phys_text_cx_cy_to_virt_text_cx_cy(uint8_t *p_virtcx, uint8_t *p_virtcy, uint8_t physcx, uint8_t physcy)
{
	uint8_t virtcx = 0u;
	uint8_t virtcy = 0u;

	if(physcx >= this->_info.n_chars) return false;
	if(physcy >= this->_info.n_lines) return false;

	virtcy = (physcy & 0x1);

	virtcx = physcx;

	physcy = (physcy >> 1);
	if(physcy) virtcx += physcy*(this->_info.n_chars);

	if(p_virtcx != NULL) *p_virtcx = virtcx;
	if(p_virtcy != NULL) *p_virtcy = virtcy;

	return true;
}

