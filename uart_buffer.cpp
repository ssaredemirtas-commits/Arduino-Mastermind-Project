
#include "uart_buffer.hpp"

void uart_init(uint32_t baud, uint8_t high_speed) {
    uint16_t ubrr = F_CPU/16/baud - 1;
    if(high_speed) {
        UBRR0H = (uint8_t)(ubrr>>8); UBRR0L = (uint8_t)ubrr;
        UCSR0A = (1<<U2X0);
    } else {
        UBRR0H = (uint8_t)(ubrr>>8); UBRR0L = (uint8_t)ubrr;
    }
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void uart_send_byte(uint8_t c) {
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = c;
}