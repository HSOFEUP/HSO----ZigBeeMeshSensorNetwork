#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#include "xbee_api.h"
#include "uart.h"
#include "bit_tools.h"

#define CTS_XBEE_ON bit_clear(PORTD,5); 		// Xbee can transmit
#define CTS_XBEE_OFF bit_set(PORTD,5); 			// (When High Xbee cannot transmit)

#define start_delimiter 0x7E	//Start delimiter
#define PEDIDO_CONFIG 101
uint8_t _data_rx[64]={0};

/*
 * MODEM STATUS
 *	0x00 = Hardware reset
 *	0x02 = Joined Network (routers and end devices)
 *	0x03 = Disassociated
 *	0x06 = Coordinator starter
 *	0x07 = Network security key was updated
 *	0x0D = Voltage supply limit exceeded (PRO S2B only)
 *	0x11 = Modem conmfiguration changed while join in progress
 *	0x80+ = stack error
	*/
uint8_t _modem_status = 0xFF;
/*
 * receive_options bit explanations
 *   0x01 -> Packet acknowledged
 *   0x02 -> Packet was a broadcast packet
 *   0x20 -> Packet encrypted with APS encryption
 *   0x40 -> Packet was sent from an end device (if known)
 */
uint8_t receive_options = 0x00;

/*
 * Error handling
 * cmd_status bit explanations
 *   0x00 -> OK
 *   0x01 -> ERROR
 *   0x02 -> Invalid Command
 *   0x03 -> Invalid Parameter
 *   0x04 ->  Tx Failure
 *   0xFF -> Unknown error
 */
uint8_t cmd_status = 0x00;

/*
 * Error handling
 * API_status bit explanations
 *   0x00 -> All OK
 *   0x01 -> AT_cmd status = error kan testas med NJ
 *   0x02 -> Frame ID error
 *   0x03 -> CRC Error
 *   0x04 -> Bad call
 *   0x05 -> Start frame error
 *   0xFF -> Unknown error
 */
uint8_t API_status = 0x00;

/*
 * Error handling
 * delivery_status bit explanations
 *	0x00 = Success
 *	0x01 = MAC ACK failure
 *	0x02 = CCA failure
 *	0x15 = Invalid destination endpoint
 *	0x21 = Network ACK failure
 *	0x22 = Not joined to network
 *	0x23 = Self-addressed
 *	0x24 = Address not found
 *	0x25 = Route not found
 *	0x26 = Broadcast source failed to hear a neighbor relay the message
 *	0x2B = Invalid binding table index
 *	0x2C = Resource error, lack of free buffers, timers, etc.
 *	0x2D = Attempted broadcast with APS transmission
 *	0x2E = Attempted unicast with APS transmission, but EE=0
 *	0x32 = Resource error, lack of free buffers, timers, etc.
 *	0x74 = Data payload too large
 *	0x75 = Indirect message unrequested	   
 *  0xFF -> Unknown error
	*/
uint8_t delivery_status = 0x00;

/*
 * Struct used to store information between API function calls
 */
static struct
{
  uint16_t length;
  uint8_t frame_id;
  uint8_t frame_type;
  char AT_cmd[2];
  uint8_t checksum;
  uint8_t AT_parameter;
} API;

uint16_t sum_of_char(char *data, uint8_t size)
{
	uint16_t sum = 0;
	for (uint8_t i=0; i < size; i++)
		{
			sum = sum + ((uint8_t)data[i]);
		}
	return sum;
}

uint16_t sum_of_16bits(uint16_t data)
{
  uint16_t sum=0;

  sum = (uint8_t)((data & 0xFF00)>>8);
  sum += (uint8_t)((data & 0x00FF));
return sum;
}

uint16_t sum_of_32bits(uint32_t data)
{
  uint16_t sum=0;

  sum = (uint8_t)((data & 0xFF000000)>>24);
  sum += (uint8_t)((data & 0x00FF0000)>>16);
  sum += (uint8_t)((data & 0x0000FF00)>>8);
  sum += (uint8_t)(data & 0x000000FF);
return sum;
}

void CRC_AT_calc()
{
	API.checksum = (0xFF - (uint8_t)(API.frame_type + API.frame_id + API.AT_cmd[0] + API.AT_cmd[1] + API.AT_parameter));
	UART_Transmit(API.checksum); // Sends CRC to module
}
uint8_t TX_req_64bit(uint32_t DL, uint32_t DH, uint8_t *data, uint8_t size) // return 0 if TX was well transmited; return 1 if not; return 2 if don't receive data from uart
{
CTS_XBEE_OFF;
	API.frame_type=0x10; 
	uint8_t tx_options=0, i, aux;				// 1 --> Disable ACK after TX
	uint16_t dest_address=0xFFFE;
	uint8_t broad_radius=0;
	
	API.frame_id++;
	if(API.frame_id == 0)                   // Frame_ID = 0 -> No response
		API.frame_id = 1;

	UART_Transmit(start_delimiter);		// Start delimiter    
	UART_Transmit_16bit(14+size);			// Length with 8 bytes of data (change this for other type of payload)
	UART_Transmit(API.frame_type);			// TX Request
	UART_Transmit(API.frame_id);			// Frame ID (for ACK)
	UART_Transmit_32bit(DH);			// Serial Number High (MSB Destination Adress)
	UART_Transmit_32bit(DL);			// Serial Number Low (LSB Destination Adress)
	UART_Transmit_16bit(dest_address);	// 16-bit destination network adress if known. 
										// Set to 0xFFFE if the address is unknown, or if sending a broadcast.
	UART_Transmit(broad_radius); 				// Broadcast radius. Sets maximum number of hops a broadcast transmission can take.
										// If set to 0, the broadcast radius will be set to the maximum hops value.
	UART_Transmit(tx_options);
	
	
	for (i=0;i<size;i++)
	{
		UART_Transmit((uint8_t)data[i]);
	}


	API.checksum = (0xFF - (uint8_t)(API.frame_type + API.frame_id + sum_of_32bits(DH) + sum_of_32bits(DL) + sum_of_16bits(dest_address) + broad_radius + tx_options + sum_of_char(data, size)) ) ;
	UART_Transmit(API.checksum);           // Sends CRC to module	

	aux = TX_status();

	while (aux == 5)
	{
		aux = TX_status();
	}

	if (aux == 0)
		return 0;
	if ( aux == (0xFF) )
		return 0xFF; // UART Receive Timeout

return 1; // ATENCAO: Dizer que foi mal transmitida significa desconeccao
}

uint8_t TX_status()
{
CTS_XBEE_ON;
	uint8_t frame_type, frame_id, delimiter, tx_retry_count, discovery_status;
	uint16_t dest_address;
	
	delimiter = UART_Receive();                             	// Start Delimiter
	if (get_crush_uart_receive() == 1) return 0xFF;
	if(delimiter != start_delimiter)                           	
	{
		API_status = 0x05;
		return 0xFE;
	}
	
	API.length = UART_Receive();             		    		// Length
	if (get_crush_uart_receive() == 1) return 0xFF;
	API.length = API.length<<8;
	API.length = API.length | UART_Receive();
	if (get_crush_uart_receive() == 1) return 0xFF;
	frame_type = UART_Receive();
	if (get_crush_uart_receive() == 1) return 0xFF;

	
	if(frame_type == 0x8A)
	{
		_modem_status=UART_Receive();
		if (get_crush_uart_receive() == 1) return 0xFF;
		API.checksum = UART_Receive();      	// Checksum
		if (get_crush_uart_receive() == 1) return 0xFF;
		if((uint8_t)(API.checksum + frame_type + _modem_status == 0xFF));
			return 5;
	}
	
	
	API.frame_type = frame_type;          	// API identifier

	frame_id = UART_Receive();                              	// Frame ID
	if (get_crush_uart_receive() == 1) return 0xFF;
	if(frame_id != API.frame_id)                               	
	{
		API_status = 0x02;                              	    
		return 0xFE;
	}
	API.frame_id = frame_id;
	
	dest_address = UART_Receive();								// 16-bit address of destination
	if (get_crush_uart_receive() == 1) return 0xFF;
	dest_address = dest_address<<8;
	dest_address = dest_address | UART_Receive();
	if (get_crush_uart_receive() == 1) return 0xFF;
	//FALTA VERIFICAR 16bit-address - caso igual ao endereço da rede foi entregue se sucesso, caso igual ao endereço de desitno --> sem sucesso
	
	tx_retry_count = UART_Receive();							// TX Retry Count
	if (get_crush_uart_receive() == 1) return 0xFF;
	
	delivery_status = UART_Receive();                           // Delivery status
	if (get_crush_uart_receive() == 1) return 0xFF;
	
	if(delivery_status > 0x75)
	{
		delivery_status = 0xFE;                         	
		return 0xFE;                                          	
	}

	discovery_status = UART_Receive();
	if (get_crush_uart_receive() == 1) return 0xFF;
	
	API.checksum = UART_Receive();                         		// Checksum
	if (get_crush_uart_receive() == 1) return 0xFF;
	if( (uint8_t)(API.checksum + API.frame_type + API.frame_id + sum_of_16bits(dest_address) + tx_retry_count + delivery_status + discovery_status) == 0xFF )
	{
		API_status = 0x00;
	}
	else
	{
		API_status = 0x03; // CRC error
		return discovery_status;
	}

return delivery_status;
}

uint8_t RX_64bit()
{
CTS_XBEE_ON;
	uint8_t delimiter, i=0;
	uint8_t size_data_rx; //Tamanho dos dados a receber
	uint16_t source_address;
	uint32_t SH=0, SL=0;
	
	delimiter = UART_Receive();					// Start delimeter
	if (get_crush_uart_receive() == 1) return 0xFF;
	if(delimiter != start_delimiter)                                	// Start condition check
		API_status = 0x05;                              // Start frame error

	API.length = UART_Receive();              			// Length (MSB)
	if (get_crush_uart_receive() == 1) return 0xFF;
	API.length = API.length<<8;
	API.length = API.length | UART_Receive(); 			// Length (LSB)
	if (get_crush_uart_receive() == 1) return 0xFF;
	API.frame_type = UART_Receive();          			// Frame type
	if (get_crush_uart_receive() == 1) return 0xFF;
	
	SH = UART_Receive();								// Source address MSB byte 1
	if (get_crush_uart_receive() == 1) return 0xFF;
	SH = SH<<8;
	SH = SH | UART_Receive();							// Source address MSB byte 2
	if (get_crush_uart_receive() == 1) return 0xFF;
	SH = SH<<8;
	SH = SH | UART_Receive();							// Source address MSB byte 3
	if (get_crush_uart_receive() == 1) return 0xFF;
	SH = SH<<8;
	SH = SH | UART_Receive();							// Source address MSB byte 4
	if (get_crush_uart_receive() == 1) return 0xFF;
	
	SL = UART_Receive();								// Source address LSB byte 1
	if (get_crush_uart_receive() == 1) return 0xFF;
	SL = SL<<8;
	SL = SL | UART_Receive();							// Source address LSB byte 2
	if (get_crush_uart_receive() == 1) return 0xFF;
	SL = SL<<8;
	SL = SL | UART_Receive();							// Source address LSB byte 3
	if (get_crush_uart_receive() == 1) return 0xFF;
	SL = SL<<8;
	SL = SL | UART_Receive();							// Source address LSB byte 4
	if (get_crush_uart_receive() == 1) return 0xFF;
	                		
	
	source_address = UART_Receive();
	if (get_crush_uart_receive() == 1) return 0xFF;
	source_address = (source_address << 8) | UART_Receive();
	if (get_crush_uart_receive() == 1) return 0xFF;
	
	receive_options = UART_Receive();                 // Receive options
	if (get_crush_uart_receive() == 1) return 0xFF;

	size_data_rx=API.length-12;				//Received data
	for(i=0;i<size_data_rx;i++)
	{
		_data_rx[i]=UART_Receive();
		if (get_crush_uart_receive() == 1) return 0xFF;
	}
   

	API.checksum = UART_Receive();            // Checksum
	if (get_crush_uart_receive() == 1) return 0xFF;

	// Checksum verification
	if( (uint8_t)(API.checksum + API.frame_type + sum_of_32bits(SH) + sum_of_32bits(SL) + sum_of_16bits(source_address) + receive_options + sum_of_char(_data_rx, size_data_rx)) == 0xFF)
	{
		API_status = 0x00;	// Checksum OK
	}
	else
	{
		API_status = 0x03; // Bad CRC
	}
return size_data_rx;
}

uint8_t TX_AT_cmd(uint16_t length)
{
CTS_XBEE_OFF;
	API.frame_id = API.frame_id+1;
	if(API.frame_id == 0)                // Frame_ID = 0 -> No response
		API.frame_id = 1;
	API.frame_type = 0x08;               // 0x08->AT Command
		
	UART_Transmit(start_delimiter);		// Start delimiter
	UART_Transmit_16bit(length);         // Length
	UART_Transmit(API.frame_type);       // API.frame_type->AT Command
	UART_Transmit(API.frame_id);         // Frame ID (for ack)
	UART_Transmit(API.AT_cmd[0]);        // First char of AT command
	UART_Transmit(API.AT_cmd[1]);        // Second char of AT command
	return 0;
}

uint8_t RX_AT_cmd()
{
CTS_XBEE_ON;
	uint8_t frame_id, delimiter, frame_type;
	
	delimiter = UART_Receive();					// Start delimeter
	if (get_crush_uart_receive() == 1) return 0xFF;
	if(delimiter !=  start_delimiter)           // Start condition check
	{
		API_status = 0x05;						// Start frame error
		return 1;								// ERROR
	}
	
	API.length = UART_Receive();               	// Length
	if (get_crush_uart_receive() == 1) return 0xFF;
	API.length = API.length<<8;
	API.length = API.length | UART_Receive();
	if (get_crush_uart_receive() == 1) return 0xFF;
	
	frame_type=UART_Receive();
	if (get_crush_uart_receive() == 1) return 0xFF;
	if(frame_type == 0x8A)
	{
		_modem_status=UART_Receive();
		if (get_crush_uart_receive() == 1) return 0xFF;
		API.checksum = UART_Receive();      	// Checksum
		if (get_crush_uart_receive() == 1) return 0xFF;
		if((uint8_t)(API.checksum + frame_type + _modem_status == 0xFF));
			return 5;
	}
	
	API.frame_type = frame_type;          	// API identifier
	frame_id = UART_Receive();
	if (get_crush_uart_receive() == 1) return 0xFF;
	                 	// Frame ID
	if(frame_id != API.frame_id)            	// Frame ID check
	{ 
		API_status = 0x02;                      // Frame ID error
		return 1;								// ERROR
	} 
	API.frame_id = frame_id;
	API.AT_cmd[0] = UART_Receive();            	// if (get_crush_uart_receive() == 1) return 0xFF;
	if (get_crush_uart_receive() == 1) return 0xFF;
	API.AT_cmd[1] = UART_Receive();            	// AT2
	if (get_crush_uart_receive() == 1) return 0xFF;
	cmd_status = UART_Receive();               	// Status
	if (get_crush_uart_receive() == 1) return 0xFF;
	if (cmd_status==0)
		return cmd_status;
	else if (cmd_status!=0)
		return cmd_status;
return 1;
}

uint8_t get_parent()
{
	uint8_t aux=0xFF;
	uint16_t cmd_data;
	API.AT_cmd[0]='M';
	API.AT_cmd[1]='P';//'MP' --> 16bit Parent Network Address;
	TX_AT_cmd(4);
	API.AT_parameter=0;
	CRC_AT_calc();
	
	aux = RX_AT_cmd();
	
	if ((aux > 0) && (aux < 5))
		{
		return 1;
		}
	while (aux==5)
	{
		aux = RX_AT_cmd();
		if ((aux > 0) && (aux < 5))
			return 1;
	}
	
	if ( aux == 0xFF)
		{CTS_XBEE_OFF; return 0xFF;}

	//Receives the parameter
	cmd_data = UART_Receive();
	if (get_crush_uart_receive() == 1) {CTS_XBEE_OFF; return 0xFF;};
	cmd_data = (cmd_data << 8) | UART_Receive();
	if (get_crush_uart_receive() == 1) {CTS_XBEE_OFF; return 0xFF;};
	
	API.checksum = UART_Receive();                        		// Checksum
	if (get_crush_uart_receive() == 1) {CTS_XBEE_OFF; return 0xFF;};
CTS_XBEE_OFF;
	if((uint8_t)(API.checksum + API.frame_type + API.frame_id + API.AT_cmd[0] + API.AT_cmd[1] + cmd_status + sum_of_16bits(cmd_data) == 0xFF))
	{
		API_status = 0x00; 
	}
	if (cmd_data!=0xFFFE)
		return 0;
	else return 1;
}

uint8_t ack_new_config(uint16_t *sleep_time_aux)
{
CTS_XBEE_ON;
	uint8_t size_data_rx;
	size_data_rx = RX_64bit();
	if ( size_data_rx == 0xFF)
		{CTS_XBEE_OFF; return 0xFF; }
CTS_XBEE_OFF;
	if (size_data_rx !=0 )
	{
		if(_data_rx[0] == 104) return 2;
		if(_data_rx[0] == 103)
		{
			*sleep_time_aux=_data_rx[1]; //ATENCAO - só recebe o tempo q dorme, 1byte
			*sleep_time_aux=(*sleep_time_aux<<8);
			*sleep_time_aux=((*sleep_time_aux)|(_data_rx[2]));
			if(*sleep_time_aux==0)
				return 1;
		return 0;
		}
	}
return 1;	
}

uint8_t get_config(uint32_t DL, uint32_t DH, uint16_t *sleep_time)
{
	uint8_t size_data_rx, data[1], result_tx;
	data[0]=PEDIDO_CONFIG;
	result_tx = TX_req_64bit(DL, DH, data, 1);
	if ( result_tx == 0xFF)
		{CTS_XBEE_OFF; return 0xFF;}
	size_data_rx = RX_64bit();
	if ( size_data_rx == 0xFF)
		{CTS_XBEE_OFF; return 0xFF;}
CTS_XBEE_OFF;
	if (size_data_rx!=0)
	{
		*sleep_time=_data_rx[0]; //ATENCAO - só recebe o tempo q dorme, 1byte
		*sleep_time=(*sleep_time<<8);
		*sleep_time=((*sleep_time)|(_data_rx[1]));
		if(*sleep_time==0)
			return 1;
	return 0;
	}	
return 1;
}


/*
uint8_t set__sleep_mode()
{
	uint8_t aux;
	API.AT_cmd[0]='S';
	API.AT_cmd[1]='M';	//'SM' --> Sleep Mode
	TX_AT_cmd(4+1);
	API.AT_parameter=1;
	UART_Transmit(API.AT_parameter);	//Pin Sleep Enable
	CRC_AT_calc();
	aux = RX_AT_cmd();
	
	if ((aux > 0) && (aux < 5))
		{
		//UART_Flush; 
		return false;}
	while (aux == 5)
	{
		aux = RX_AT_cmd();
		if ((aux > 0) && (aux < 5))
			return false;
	}
	
	API.checksum = UART_Receive();                         		// Checksum
	if((uint8_t)(API.checksum + API.frame_type + API.frame_id + API.AT_cmd[0] + API.AT_cmd[1] + cmd_status == 0xFF))
	{
		API_status = 0x00; 
	}
	
	if (cmd_status==0)
		return 0;
	else return 1;
}

//REMOTE AT inc.
TX_remote_AT_cmd (DH,DL)
{
	uint16_t dest_address=0xFFFE;
	uint8_t remote_options=0x02;				// 1 --> Apply changes on remote
	API.frame_id = API.frame_id++;
	if(API.frame_id == 0)                // Frame_ID = 0 -> No response
		API.frame_id = 1;
	API.frame_type = 0x17;               // Remote AT Command Request
		
	UART_Transmit(start_delimiter);		// Start delimiter
	UART_Transmit_16bit(length);         // Length
	UART_Transmit(API.frame_type);       // API.frame_type
	UART_Transmit(API.frame_id);       // API.frame_id
	UART_Transmit_32bits(DH);			// 64-bit Destination Address
	UART_Transmit_32bits(DL);
	UART_Transmit_16bits(dest_address)	// 16-bit Destination Address
	UART_Transmit(remote_options);
	UART_Transmit(API.AT_cmd[0]);        // First char of AT command
	UART_Transmit(API.AT_cmd[1]);        // Second char of AT command
	
	if (lenght>4)
	{
	
	}
	
	API.checksum = (0xFF - (uint8_t)(API.frame_type + API.frame_id + API.AT_cmd[0] + API.AT_cmd[1]));
	UART_Transmit(API.checksum); // Sends CRC to module

	return API.frame_id; //PARA???????

}
*/
