
#include "HD44780.hpp"


#define LCD_DATA_PORT PORTD
#define LCD_DATA_DDR  DDRD
#define LCD_DATA_PIN  PIND
#define LCD_RS_PIN    PB0 
#define LCD_RW_PIN    PB1  
#define LCD_E_PIN     PB1  

void LCD_WriteCommand(unsigned char commandToWrite) {
    PORTB &= ~(1<<LCD_RS_PIN); 
    
    PORTD = (PORTD & 0x0F) | (commandToWrite & 0xF0);
    PORTB |= (1<<LCD_E_PIN); asm volatile ("nop"); PORTB &= ~(1<<LCD_E_PIN);
    _delay_us(50);
    
    PORTD = (PORTD & 0x0F) | ((commandToWrite << 4) & 0xF0);
    PORTB |= (1<<LCD_E_PIN); asm volatile ("nop"); PORTB &= ~(1<<LCD_E_PIN);
    _delay_us(50);
}

void LCD_WriteData(unsigned char dataToWrite) {
    PORTB |= (1<<LCD_RS_PIN); 
    
    PORTD = (PORTD & 0x0F) | (dataToWrite & 0xF0);
    PORTB |= (1<<LCD_E_PIN); asm volatile ("nop"); PORTB &= ~(1<<LCD_E_PIN);
    _delay_us(50);
   
    PORTD = (PORTD & 0x0F) | ((dataToWrite << 4) & 0xF0);
    PORTB |= (1<<LCD_E_PIN); asm volatile ("nop"); PORTB &= ~(1<<LCD_E_PIN);
    _delay_us(50);
}

void LCD_Initalize(void) {
    DDRB |= (1<<LCD_RS_PIN) | (1<<LCD_E_PIN);
    DDRD |= 0xF0; 
    _delay_ms(15);
    LCD_WriteCommand(0x03); _delay_ms(5);
    LCD_WriteCommand(0x03); _delay_us(100);
    LCD_WriteCommand(0x03);
    LCD_WriteCommand(0x02); 
    LCD_WriteCommand(0x28); 
    LCD_WriteCommand(0x0C); 
    LCD_WriteCommand(0x06); 
    LCD_Clear();
}

void LCD_Clear(void) {
    LCD_WriteCommand(0x01);
    _delay_ms(2);
}

void LCD_GoTo(unsigned char x, unsigned char y) {
    unsigned char addr = 0x80 + x + (y * 0x40);
    LCD_WriteCommand(addr);
}

void LCD_WriteText(char *text) {
    while(*text) LCD_WriteData(*text++);
}