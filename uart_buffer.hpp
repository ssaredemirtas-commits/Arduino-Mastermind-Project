
#ifndef UART_BUFFER_HPP_
#define UART_BUFFER_HPP_
#include <avr/io.h>
#include <stdint.h>

void uart_init(uint32_t baud, uint8_t high_speed);
void uart_send_byte(uint8_t c);

#endif