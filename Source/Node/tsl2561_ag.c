#include "tools.h"
#include "uart.h"

int tsl2561_init(uint8_t addr)
{
uint8_t aux1=0xFF, aux2=0xFF, aux3=0xFF;
	I2C_start();
	aux1=I2C_sendAddr(addr); // modo escrita
	aux2=I2C_sendData(0x81); // Timing Register, 
	aux3=I2C_sendData(TSL_TINT); // integration time of 0x02 402ms, 0x01 101ms, 0x00 13.7ms
	I2C_stop();
/*
UART_Transmit(aux1);
UART_Transmit(aux2);
UART_Transmit(aux3);
UART_Transmit(0xFA);
*/
	I2C_start();
	aux1=I2C_sendAddr(addr); // modo escrita
	aux2=I2C_sendData(0x86); // Interrpt Control Resgister
	aux3=I2C_sendData(0x00); // 00 Disable Interrupt | 0x10 Every ADC cycle generate one interrupt
	I2C_stop();

	I2C_start();
	aux1=I2C_sendAddr(addr); // modo escrita
	aux2=I2C_sendData(0x80); // Control Resgister - start continuous measures
	aux3=I2C_sendData(0x03); // Powered Up
	I2C_stop();
return 0;	
}

uint16_t tsl2561_read_ch0(uint8_t addr)
{
uint8_t data0=0, data1=1, aux1=0x9C, aux2=0x9C;
	I2C_start();
	aux1=I2C_sendAddr(addr); // modo escrita
	aux2=I2C_sendData(0x8C); // comando LSB

	I2C_repeatStart();
    aux1=I2C_sendAddr(addr+1);
	data0=I2C_receiveData_NACK();
	I2C_stop();

   	I2C_start();
	I2C_sendAddr(addr); // modo escrita
	I2C_sendData(0x8D); // comando MSB

	I2C_start(); 
    I2C_sendAddr(addr+1); // comando MSB
	data1=I2C_receiveData_NACK();
	I2C_stop();

return ((data1<<8) + (data0));	
}

uint16_t tsl2561_read_ch1(uint8_t addr)
{
uint8_t data0, data1;
	I2C_start();
	I2C_sendAddr(addr); // modo escrita
	I2C_sendData(0x8E); // comando LSB

	I2C_repeatStart();
    I2C_sendAddr(addr+1);
	data0=I2C_receiveData_NACK();
	I2C_stop();

   	I2C_start();
	I2C_sendAddr(addr); // modo escrita
	I2C_sendData(0x8F); // comando MSB

	I2C_start(); 
    I2C_sendAddr(addr+1); // comando MSB
	data1=I2C_receiveData_NACK();
	I2C_stop();
return ((data1<<8) + (data0));
}

// lux equation approximation without floating point calculations
// Routine: unsigned int tsl2561_calc_lux(uint16_t ch0, uint16_t ch1, int iType)
//
// Description: Calculate the approximate illuminance (lux) given the raw
// channel values of the TSL2561. The equation if implemented
// as a piecewise linear approximation.
//
// Arguments: unsigned int iGain - gain, where 0:1X, 1:16X
// unsigned int tInt - integration time, where 0:13.7mS, 1:100mS, 2:401mS,
// 3:Manual
// unsigned int ch0 - raw channel value from channel 0 of TSL2561
// unsigned int ch1 - raw channel value from channel 1 of TSL2561
// unsigned int iType - package type (T->0 or CS-1)
//
// Return: unsigned int - the approximate illuminance (lux)
//
//////////////////////////////////////////////////////////////////////////////
uint16_t tsl2561_calc_lux(uint16_t iGain, uint16_t tInt, uint16_t ch0,uint16_t ch1)
{ 
// First, scale the channel values depending on the gain and integration time    
// 16X, 402mS is nominal.    
// scale if integration time is NOT 402 msec    
unsigned long chScale;   
unsigned long channel1;   
unsigned long channel0;   
unsigned long temp;   
unsigned long lux;   
unsigned long ratio1 = 0;   
unsigned long ratio;    
unsigned int b, m;   
    switch (tInt)   
        {   
        case 0: chScale = CHSCALE_TINT0;   
                break;                      // 13.7 msec    
        case 1: chScale = CHSCALE_TINT1;   
                break;                      // 101 msec    
        default:chScale = (1 << CH_SCALE);   
                break;                      // assume no scaling    
        }   
    if (!iGain)                             // scale if gain is NOT 16X    
        chScale = chScale << 4;           	// scale 1X to 16X    
                                            // scale the channel values    
    channel0 = (ch0 * chScale) >> CH_SCALE;   
    channel1 = (ch1 * chScale) >> CH_SCALE;   
// find the ratio of the channel values (Channel1/Channel0)    
// protect against divide by zero    
    if (channel0 != 0)    
        ratio1 = (channel1 << (RATIO_SCALE+1)) / channel0;   
    ratio = (ratio1 + 1) >> 1;                    // round the ratio value    
   
    if ((ratio >= 0) && (ratio <= K1T))           // is ratio <= eachBreak ?    
        {b=B1T; m=M1T;}   
    else    
        if (ratio <= K2T)   
            {b=B2T; m=M2T;}   
        else    
            if (ratio <= K3T)   
                {b=B3T; m=M3T;}   
            else    
                if (ratio <= K4T)   
                    {b=B4T; m=M4T;}   
                else    
                    if (ratio <= K5T)   
                        {b=B5T; m=M5T;}   
                    else   
                        if (ratio <= K6T)   
                            {b=B6T; m=M6T;}   
                        else   
                            if (ratio <= K7T)   
                                {b=B7T; m=M7T;}   
                            else    
                                if (ratio > K8T)   
                                    {b=B8T; m=M8T;}   
    temp = ((channel0 * b) - (channel1 * m));   
    if (temp < 0)                            // do not allow negative lux value    
        temp = 0;   
    temp += (1 << (LUX_SCALE-1));         // round lsb (2^(LUX_SCALE.1))    
   
    lux = temp >> LUX_SCALE;              // strip off fractional portion    
    return(lux);   
}
