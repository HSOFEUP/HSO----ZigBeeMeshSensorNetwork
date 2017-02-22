#include "uart.h"

uint8_t crush_uart_receive=0;
volatile uint8_t timeout_timer0=0;

ISR(TIMER0_OVF_vect)
{
   //This is the interrupt service routine for TIMER0 OVERFLOW Interrupt.
   //CPU automatically call this when TIMER0 overflows.

   //Increment our variable
   timeout_timer0++;
   if(timeout_timer0>30)
   {
      timeout_timer0=0;
   }
//UART_Transmit(timeout_timer0);
//UART_Transmit(timeout_timer0);
}

uint8_t get_crush_uart_receive()
{
uint8_t aux_crush=0;
	aux_crush=crush_uart_receive;
	crush_uart_receive=0;
return aux_crush;
}

void UART_Init(unsigned int baud)// Configura a porta série:ubrr=12-->9600,N,8,1
{
/* Coloca o Baud Rate */
UBRRH = (unsigned char)(baud>>8);
UBRRL = (unsigned char) baud;

/* Habilita a Recepção e a Transmissão */
UCSRB = (1<<RXEN)|(1<<TXEN);//|(1<<RXCIE);
/* Define o formato: 8data, 1 bit stop */ 
UCSRC = (1<<URSEL)|(0<<USBS)|(3<<UCSZ0);
}

void UART_Transmit(uint8_t data) // Envia pela porta série o valor contido em data
{
/* Wait for empty transmit buffer */
while ( !( UCSRA & (1<<UDRE)) );
/* Put data into buffer, sends the data */
UDR = data;

while((UCSRA & (1<<TXC)) == 0); //Transmission complete 
UCSRA |= (1<<TXC);	

}

uint8_t UART_Receive(void) //Recebe um byte da porta série
{
	TCNT0 = 0x00;
	TIMSK|=(1<<TOIE0); // Enable Overflow Interrupt Enable
	timeout_timer0=0;

/* Wait for data to be received */
while ( !(UCSRA & (1<<RXC)) && (timeout_timer0 < 30) );
/* Get and return received data from buffer */
	if ( timeout_timer0>9 )
		{
			crush_uart_receive=1; 
			return 0xFF;
		}
	TIMSK|=(0<<TOIE0); // Disable Overflow Interrupt Enable
return UDR;
}


void UART_Flush(void)
{
	unsigned char dummy;
	while (UCSRA & (1<<RXC))
		dummy = UDR;
}

void UART_Transmit_32bit(uint32_t data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put MSB_high of data into buffer, sends the data */
	UDR = (uint8_t)((data & 0xFF000000)>>24);

	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put MSB_low of data into buffer, sends the data */
	UDR = (uint8_t)((data & 0x00FF0000)>>16);

	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put LSB_high of data into buffer, sends the data */
	UDR = (uint8_t)((data & 0x0000FF00)>>8);

	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put LSB_low of data into buffer, sends the data */
	UDR = (uint8_t)((data & 0x000000FF));
	
	/* Transmission complete */
	while((UCSRA & (1<<TXC)) == 0);
	UCSRA |= (1<<TXC);
}

void UART_Transmit_16bit(uint16_t data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put MSB of data into buffer, sends the data */
	UDR = (uint8_t)((data & 0xFF00)>>8);

	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put LSB of data into buffer, sends the data */
	UDR = (uint8_t)((data & 0x00FF));

	/* Transmission complete */
	while((UCSRA & (1<<TXC)) == 0);
	UCSRA |= (1<<TXC);
}


