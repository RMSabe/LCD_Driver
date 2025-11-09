/*
  Driver Test 1
  Version 1.0

  Author: Rafael Sabe
  Email: rafaelmsabe@gmail.com
*/

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <lcd.hpp>

#define TEXTBUF_SIZE_CHARS 256U
#define TEXTBUF_SIZE_BYTES TEXTBUF_SIZE_CHARS

#define LCD_DB4 20U
#define LCD_DB5 21U
#define LCD_DB6 22U
#define LCD_DB7 23U
#define LCD_RS 14U
#define LCD_E 30U
#define LCD_NCHARS 20U
#define LCD_NLINES 4U

char textbuf[TEXTBUF_SIZE_CHARS];
LCD lcd1(LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7, LCD_RS, LCD_E, LCD_NCHARS, LCD_NLINES);

void setup(void)
{
  memset(textbuf, 0, TEXTBUF_SIZE_BYTES);
  
  lcd1.begin();

  lcd1.setCursorPosition(0u, 0u);
  lcd1.printText("This is line 00");
  lcd1.setCursorPosition(0u, 1u);
  lcd1.printText("This is line 01");
  lcd1.setCursorPosition(0u, 2u);
  lcd1.printText("This is line 02");
  lcd1.setCursorPosition(0u, 3u);
  lcd1.printText("This is line 03");

  delay(4096u);

  lcd1.clear();
  lcd1.home();

  lcd1.printText("Counting... ");
  
  return;
}

void loop(void)
{
  uint8_t b = 0u;

_l_loop_runtimeloop:

  snprintf(textbuf, TEXTBUF_SIZE_CHARS, "%u  ", b);
  lcd1.setCursorPosition(12u, 0u);
  lcd1.printText(textbuf);
  
  delay(1024u);
  b++;

  goto _l_loop_runtimeloop;
  return;
}
