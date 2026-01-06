

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h> 
#include "Arduino.h" 


#include "HD44780.hpp"
#include "libADC.hpp"
#include "uart_buffer.hpp"


#define LED_PORT PORTB
#define LED_DDR  DDRB
#define LED1_PIN PB2
#define LED2_PIN PB3
#define LED3_PIN PB4
#define LED4_PIN PB5


#define BTN_PIN  PIND
#define BTN_1    PD0
#define BTN_2    PD1
#define BTN_3    PD2
#define BTN_4    PD3


int secretCode[4];       
int playerGuess[4];      
int currentDigit = 0;    
int maxAttempts = 10;    
int currentAttempt = 0;
bool gameActive = false;
bool win = false;

char lcdBuffer[16];


void showLedFeedback(int count) {
    LED_PORT &= ~((1 << LED1_PIN) | (1 << LED2_PIN) | (1 << LED3_PIN) | (1 << LED4_PIN));
    _delay_ms(200);
    
    if (count >= 1) LED_PORT |= (1 << LED1_PIN);
    if (count >= 2) LED_PORT |= (1 << LED2_PIN);
    if (count >= 3) LED_PORT |= (1 << LED3_PIN);
    if (count >= 4) LED_PORT |= (1 << LED4_PIN);
    
    _delay_ms(1000); 
    LED_PORT &= ~((1 << LED1_PIN) | (1 << LED2_PIN) | (1 << LED3_PIN) | (1 << LED4_PIN));
}


void generateSecretCode() {
    uint16_t seed = ADC_conversion(); 
    srand(seed);
    for (int i = 0; i < 4; i++) {
        secretCode[i] = (rand() % 4) + 1; 
    }
}


void checkSolution() {
    int exactMatch = 0;
    int wrongPlace = 0;
    int secretCopy[4];
    int guessCopy[4];
    
    for(int i=0; i<4; i++) {
        secretCopy[i] = secretCode[i];
        guessCopy[i] = playerGuess[i];
    }

    
    for (int i = 0; i < 4; i++) {
        if (guessCopy[i] == secretCopy[i]) {
            exactMatch++;
            secretCopy[i] = -1; 
            guessCopy[i] = -1;
        }
    }

    
    for (int i = 0; i < 4; i++) {
        if (guessCopy[i] == -1) continue; 
        for (int j = 0; j < 4; j++) {
            if (secretCopy[j] == -1) continue; 
            if (guessCopy[i] == secretCopy[j]) {
                wrongPlace++;
                secretCopy[j] = -1; 
                break;
            }
        }
    }

    sprintf(lcdBuffer, "EXACT:%d NEAR:%d", exactMatch, wrongPlace);
    LCD_GoTo(0, 1);
    LCD_WriteText(lcdBuffer);
    showLedFeedback(exactMatch);

    if (exactMatch == 4) {
        win = true;
        gameActive = false;
    }
}

int main() {
    init(); 
    LCD_Initalize();
    ADC_Init();
    
    
    LED_DDR |= (1 << LED1_PIN) | (1 << LED2_PIN) | (1 << LED3_PIN) | (1 << LED4_PIN);
    
    
    DDRD &= ~((1 << BTN_1) | (1 << BTN_2) | (1 << BTN_3) | (1 << BTN_4)); 
    PORTD |= (1 << BTN_1) | (1 << BTN_2) | (1 << BTN_3) | (1 << BTN_4); 

    
    LCD_Clear();
    LCD_GoTo(0,0);
    LCD_WriteText((char*)"MASTERMIND GAME");
    LCD_GoTo(0,1);
    LCD_WriteText((char*)"Cracking Code...");
    _delay_ms(1500);

    uint8_t lastBtn1 = 1, lastBtn2 = 1, lastBtn3 = 1, lastBtn4 = 1;

    while (1) {
        
        if (!gameActive) {
            uint16_t potVal = ADC_conversion();
            int settingAttempts = 5 + (potVal / 100); 
            
            sprintf(lcdBuffer, "LIVES:%2d PRESS 1", settingAttempts);
            LCD_GoTo(0,0);
            LCD_WriteText(lcdBuffer);
            LCD_GoTo(0,1);
            LCD_WriteText((char*)"TO START GAME   ");

            
            if (!(PIND & (1 << BTN_1))) {
                generateSecretCode();
                maxAttempts = settingAttempts;
                currentAttempt = 1;
                currentDigit = 0;
                gameActive = true;
                win = false;
                
                LCD_Clear();
                LCD_GoTo(0,0);
                LCD_WriteText((char*)"GUESS: _ _ _ _");
                _delay_ms(500);
            }
            _delay_ms(100);
            continue;
        }

        
        int input = 0;
        if (!(PIND & (1 << BTN_1)) && lastBtn1) input = 1;
        lastBtn1 = (PIND & (1 << BTN_1));

        if (!(PIND & (1 << BTN_2)) && lastBtn2) input = 2;
        lastBtn2 = (PIND & (1 << BTN_2));

        if (!(PIND & (1 << BTN_3)) && lastBtn3) input = 3;
        lastBtn3 = (PIND & (1 << BTN_3));

        if (!(PIND & (1 << BTN_4)) && lastBtn4) input = 4;
        lastBtn4 = (PIND & (1 << BTN_4));

        if (input > 0) {
            playerGuess[currentDigit] = input;
            LCD_GoTo(7 + (currentDigit * 2), 0); 
            char numChar[2];
            sprintf(numChar, "%d", input);
            LCD_WriteText(numChar);
            currentDigit++;
            _delay_ms(200); 
        }

        
        if (currentDigit >= 4) {
            checkSolution();
            if (win) {
                LCD_Clear();
                LCD_GoTo(0,0); LCD_WriteText((char*)"*** HACKED! ***");
                LCD_GoTo(0,1); LCD_WriteText((char*)"CODE FOUND!     ");
                for(int k=0; k<5; k++) { LED_PORT = 0xFF; _delay_ms(100); LED_PORT = 0x00; _delay_ms(100); }
                gameActive = false;
            } else {
                currentAttempt++;
                currentDigit = 0;
                if (currentAttempt > maxAttempts) {
                    LCD_Clear();
                    LCD_GoTo(0,0); LCD_WriteText((char*)"GAME OVER (X_X)");
                    sprintf(lcdBuffer, "CODE: %d %d %d %d", secretCode[0], secretCode[1], secretCode[2], secretCode[3]);
                    LCD_GoTo(0,1); LCD_WriteText(lcdBuffer);
                    gameActive = false;
                    _delay_ms(3000);
                } else {
                    _delay_ms(1500);
                    LCD_Clear();
                    sprintf(lcdBuffer, "TRY %d/%d", currentAttempt, maxAttempts);
                    LCD_GoTo(0,1); LCD_WriteText(lcdBuffer);
                    LCD_GoTo(0,0); LCD_WriteText((char*)"GUESS: _ _ _ _");
                }
            }
        }
        _delay_ms(10);
    }
    return 0;
}