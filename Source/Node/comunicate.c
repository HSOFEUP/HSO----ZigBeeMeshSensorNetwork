#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr\interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h> //libraria para podermos utilizar a escrita e leitura 
#include <util/delay.h>
#include "bit_tools.h"
#include "uart.h"
#include "xbee_api.h"
#include "twi.h"
#include "tools.h"

#define F_CPU 2000000UL

#define SLEEP_TIME_DEF 5
#define SIZE_WINDOW 9 // SOUND SENSOR - Maximum window size of sound samples
#define SIZE_SENSORS_TX 10

/***********DEFINE I/O**********/
#define	SLEEP_XBEE_ON	bit_set(PORTC, PC1);	// Xbee Pin Mode Sleep
#define	SLEEP_XBEE_OFF	bit_clear(PORTC, PC1);
#define CTS_XBEE_ON bit_clear(PORTD,5); 		// Xbee can transmit
#define CTS_XBEE_OFF bit_set(PORTD,5); 			// (When High Xbee cannot transmit)
#define TSL2561_POWER_UP bit_set(PORTB,2); 		// TSL2561 ON
#define TSL2561_POWER_DOWN bit_clear(PORTB,2); 	// TSL2561 OFF
#define SOUND_SENSOR_ON bit_set(PORTD, PD7); 	// Vcc to Sound Sensor
#define SOUND_SENSOR_OFF bit_clear(PORTD, PD7); 	// Turn off Sound Sensor
#define	V_AREF_ON	bit_set(PORTB, 0);			// Vref ADC (Potenciometer)
#define	V_AREF_OFF	bit_clear(PORTB, 0);

volatile uint16_t count_sleep=0;
uint16_t normal_mode=0, sleep_time=1, sleep_time_aux;
uint32_t DH=0x000000000000, DL=0x000000000000;

void init (void)
{
	OSCCAL=0xD2; 								// To calibrate Internal RC
	// INIT_T2
	bit_set(ASSR,AS2); 							// Enable asynchronous mode
	bit_set(TIMSK,TOIE2); 						// Timer/Counter 2 Overflow Interrupt Enable
	bit_set(TIFR,TOV2); 						// Clear TOV2
	TCNT2=0; 									// Init Timer Count 2
	TCCR2 = ((1<<CS22)|(0<<CS21)|(1<<CS20)); 	// Timer Clock = system clock / 128 => TIOV=1s
	while(ASSR & _BV(TCN2UB));            		// Wait until TC2 is updated 

	//init AD --> ADC0
	ADMUX = ((0<<REFS1)|(0<<REFS0)|(0<<MUX3)|((0<<MUX2))|(0<<MUX1)); 	//AVCCwith external capacitor at AREF pin & ADC0
	ADCSRA = ((1<<ADEN)|(0<<ADFR)|(1<<ADPS2)|(0<<ADPS1)|(0<<ADPS0));  	//ADC clock=systemclock/16 --> 125KHz

	//INIT Timer0
	TCCR0 = ((1<<CS02)|(0<<CS01)|(1<<CS00)); // Timer Clock = system clock / 1024 => TIOV=0,131s
	//bit_set(TIMSK,TOIE0); //Timer/Counter 0 Overflow Interrupt Enable
	TCNT0 = 0x00;     // Init Timer Count 0
}

void sleepNow()		// here we put the atmega8 to sleep
{
	set_sleep_mode(SLEEP_MODE_PWR_SAVE);   	// sleep mode is set here
	sleep_enable();
	sleep_mode();							// here the device is actually put to sleep!!
                   							// THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
	sleep_disable();						// first thing after waking from sleep:
}

ISR(TIMER2_OVF_vect)
{
	count_sleep++;
}

void tsl2561_start()
{	
	//After wake-up it's needed 
	//to clearing and then resetting the TWEN and the TWSTO bit
	TWCR =((0<<TWSTO)|(1<<TWEN));
	TWCR =(0<<TWEN); 

	TSL2561_POWER_UP;
	_delay_ms(4);
	tsl2561_init(TSL_ADDRESS);
	_delay_ms(200);
}

void tsl2561_end()
{
	TSL2561_POWER_DOWN;
}

uint16_t read_sound_preasure(uint8_t size_window, uint8_t number_samples, uint8_t exp_base_2) //Read RMS value of sound pressuare. Attention: 2^exp_base_2=number_samples; filtro passa-baixo com a média e com a mediana tiro o valor que mais representa a amostra; perco no mínimo um 1bit
{
uint8_t i, j, k;
uint32_t sum=0;
uint16_t vect_average[SIZE_WINDOW], temp;
	for(i=0; i<size_window; i++)
	{
		for (j=0; j<number_samples; j++)
		{
			ADCSRA |= (1<<ADSC);
			while (ADCSRA &  (1<<ADSC)); 	// wait until conversion  completes; ADSC=0 means Complete
			sum = sum + ADC;				//Store ADC Value
		}
		vect_average[i]=sum>>exp_base_2;
		sum=0;
		
		if (i!=0) 							// Ascending sort vector
		{
			for(j=0; j==i;j++)
			{
				if (vect_average[i]<vect_average[j])
				{
					temp=vect_average[i];
					for(k=i;k==j+1;k--)
						vect_average[k]=vect_average[k-1];
					vect_average[j]=temp;
				}
			}			
		}			

	}
return(vect_average[(size_window>>2)+1]);
}

int main(void)
{
//INICIALIZACOES
uint8_t data_tx[SIZE_SENSORS_TX], res_xbee_func;
uint16_t full_light_value, visible_light_value, ir_light_value, lux_light_value, dba_sound_value;
UART_Init(12); 		// UBRR=12 --> BR=9600 c/ fosc=2MHz
init();
I2C_init();			// Inicia i2c define velocidade do bitrate

bit_set(DDRB, PB0); // Pin PB0 is an output (Varef)
//bit_set(DDRB, PB1); // Pin PB1 is an output
bit_set(DDRB, PB2); // Vcc to TSL2561
bit_set(DDRC, PC1); // SLEEP_XBEE (When High Xbee sleep)
bit_set(DDRD, PD5); // XBEE(PIN 16 - RTS)
bit_set(DDRD, PD7); // Vcc to Sound Sensor

//bit_clear(PORTB, 0);
bit_clear(PORTB, 1);

CTS_XBEE_OFF;

_delay_ms (1000);

count_sleep=0;
sei(); 				//enable_interrupts() <=> bit_set(SREG,7)

while(1)
{
	while(normal_mode==0)
	{
	bit_toggle(PORTB,1);
		if(count_sleep >= SLEEP_TIME_DEF)
		{
		bit_clear(TIMSK,TOIE2); // Dessativa interrupcao Timer2
		SLEEP_XBEE_OFF;			// Acorda o modulo Xbee
		_delay_ms (10);
			if(get_parent() == 0)
			{
				res_xbee_func = get_config(DL,DH, &sleep_time);
				
				if ( res_xbee_func == 1 ) // Ainda não tem configuração;
				{
					sleep_time = SLEEP_TIME_DEF;
					normal_mode=1;
				} 
				else if (res_xbee_func == 0)
					normal_mode=1;
			}
		count_sleep=0;
		TCNT2=0; // Reset Timer 2
		while(ASSR & _BV(TCN2UB));
		bit_set(TIMSK,TOIE2); // Ativa interrupcao Timer2
		}
	SLEEP_XBEE_ON; //
	sleepNow();	// uC dorme
	}

	while (normal_mode==1)
	{
		if(count_sleep >= sleep_time)
		{
			bit_clear(TIMSK,TOIE2); // Disable interrupt timer 2
			V_AREF_ON;				// Ativa Vref ADC
			SOUND_SENSOR_ON;		// Liga o TSL2561 (Sensor de luz)
			//Read Light Sensor
			tsl2561_start();
			full_light_value = tsl2561_read_ch0(TSL_ADDRESS);
			ir_light_value = tsl2561_read_ch1(TSL_ADDRESS);
			lux_light_value = tsl2561_calc_lux(0, TSL_TINT, full_light_value, ir_light_value);
			tsl2561_end();
			visible_light_value = full_light_value - ir_light_value;
			data_tx[0]=1; // ID_Sensor Luz
			data_tx[1]=(uint8_t)(visible_light_value>>8);
			data_tx[2]=(uint8_t)(visible_light_value);
			data_tx[3]=(uint8_t)(ir_light_value>>8);
			data_tx[4]=(uint8_t)(ir_light_value);
			data_tx[5]=(uint8_t)(lux_light_value>>8);
			data_tx[6]=(uint8_t)(lux_light_value);
			//Read Sound Sensor
			dba_sound_value=read_sound_preasure(5,8,3);
			SLEEP_XBEE_OFF;			// Acorda modulo Xbee
			SOUND_SENSOR_OFF;
			V_AREF_OFF;
			data_tx[7] = 2;
			data_tx[8]=(uint8_t)(dba_sound_value>>8);
			data_tx[9]=(uint8_t)(dba_sound_value);
			res_xbee_func = TX_req_64bit(DL, DH, data_tx, SIZE_SENSORS_TX);
			if (res_xbee_func == 1) //Enviar mensaegem de erro
				{CTS_XBEE_OFF;}
			else if (res_xbee_func == 0xFF);
				{CTS_XBEE_OFF}
			//RECEBER INFORMACAO DE NOVA CONFIGURACAO
			res_xbee_func = ack_new_config(&sleep_time_aux);
			if(res_xbee_func==0)
				sleep_time=sleep_time_aux;
			if(res_xbee_func==1)
				sleep_time=SLEEP_TIME_DEF;
			if(res_xbee_func==0xFF)
				{sleep_time=SLEEP_TIME_DEF; normal_mode=0;}
		count_sleep=0;
		bit_set(TIMSK,TOIE2); // Ativa interrupcao Timer2
		TCNT2=0; // Reset Timer 2
		while(ASSR & _BV(TCN2UB));
		}
	SLEEP_XBEE_ON; 
	sleepNow();
	}
}
}
