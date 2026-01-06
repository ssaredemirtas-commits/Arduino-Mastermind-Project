
#ifndef HD44780_HPP_
#define HD44780_HPP_

#include <avr/io.h>
#include <util/delay.h>

void LCD_WriteCommand(unsigned char commandToWrite);
void LCD_WriteData(unsigned char dataToWrite);
void LCD_WriteText(char *text);
void LCD_GoTo(unsigned char x, unsigned char y);
void LCD_Clear(void);
void LCD_Initalize(void);

#endif