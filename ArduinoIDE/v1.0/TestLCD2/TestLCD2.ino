/*
  Driver Test 2
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

#define LCD1_E 30U
#define LCD1_NCHARS 20U
#define LCD1_NLINES 4U

#define LCD2_E 29U
#define LCD2_NCHARS 16U
#define LCD2_NLINES 2U

#define LOOP_DELAYTIME_MS 512U

char textbuf[TEXTBUF_SIZE_CHARS];
LCD lcd1(LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7, LCD_RS, LCD1_E, LCD1_NCHARS, LCD1_NLINES);
LCD lcd2(LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7, LCD_RS, LCD2_E, LCD2_NCHARS, LCD2_NLINES);

uint16_t num16 = 0u;

void setup(void)
{
  memset(textbuf, 0, TEXTBUF_SIZE_BYTES);

  lcd1.begin();
  lcd2.begin();

  lcd1.setCursorPosition(0u, 0u);
  lcd1.printText("This is line 00");
  lcd1.setCursorPosition(0u, 1u);
  lcd1.printText("This is line 01");
  lcd1.setCursorPosition(0u, 2u);
  lcd1.printText("This is line 02");
  lcd1.setCursorPosition(0u, 3u);
  lcd1.printText("This is line 03");

  lcd2.setCursorPosition(0u, 0u);
  lcd2.printText("This is line 0");
  lcd2.setCursorPosition(0u, 1u);
  lcd2.printText("This is line 1");

  delay(4096u);

  lcd1.clear();
  lcd1.home();
  lcd1.printText("Counting...");

  lcd2.clear();
  lcd2.home();
  lcd2.printText("Counting...");
  
  return;
}

void loop(void)
{
_l_loop_runtimeloop:

  snprintf(textbuf, TEXTBUF_SIZE_CHARS, "%u    ", num16);
  lcd1.setCursorPosition(12u, 0u);
  lcd1.printText(textbuf);

  snprintf(textbuf, TEXTBUF_SIZE_CHARS, "%u  ", (num16 & 0xff));
  lcd2.setCursorPosition(12u, 0u);
  lcd2.printText(textbuf);

  delay(LOOP_DELAYTIME_MS);
  num16++;

  goto _l_loop_runtimeloop;
  return;
}
