#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

uint8_t get_crush_uart_receive();
void UART_Init(unsigned int baud);
void UART_Transmit(uint8_t data);
uint8_t UART_Receive(void);
void UART_Flush(void);
void UART_Transmit_32bit(uint32_t data);
void UART_Transmit_16bit(uint16_t data);
