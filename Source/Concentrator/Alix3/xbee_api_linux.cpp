/*
 * xbee_api_linux.c
 *
 *   These routine developed by Mario de Sous
 */

#include "xbee_api_linux.h"

#define start_delimiter 0x7E	//Start delimiter
#define timeout 1

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
} API;

uint16_t sum_of_char(uint8_t *data, uint8_t size)
{
	uint16_t sum = 0;
	uint8_t i;
	for (i=0; i < size; i++)
		{
			sum = sum + ((uint8_t)data[i]);
		}
	return sum;
}

uint16_t sum_of_16bits(uint16_t data)
{
  uint16_t sum;

  sum = (uint8_t)((data & 0xFF00)>>8);
  sum += (uint8_t)((data & 0x00FF));
return sum;
}

uint16_t sum_of_32bits(uint32_t data)
{
  uint16_t sum;

  sum = (uint8_t)((data & 0xFF000000)>>24);
  sum += (uint8_t)((data & 0x00FF0000)>>16);
  sum += (uint8_t)((data & 0x0000FF00)>>8);
  sum += (uint8_t)(data & 0x000000FF);
return sum;
}

bool TX_req_64bit(int fd, uint32_t DL, uint32_t DH, uint8_t *data, uint8_t size)
{
	uint8_t tx_options=0, i, w_serial;				// 1 --> Disable ACK after TX
	uint16_t dest_address=0xFFFE;
	uint8_t broad_radius=0;
	uint8_t frame_size = size + 18;
	uint8_t *frame;

	frame=(uint8_t *)malloc(frame_size*sizeof(uint8_t));

	API.frame_type=0x10;
	API.frame_id++;
	if(API.frame_id == 0)                   // Frame_ID = 0 -> No response
		API.frame_id = 1;

	frame[0] = start_delimiter;		// Start delimiter
	frame[1] = 0;					// MSB Length
	frame[2] = 14+size;			// LSB Length (change this for other type of payload)
	frame[3] = API.frame_type;			// TX Request
	frame[4] = API.frame_id;			// Frame ID (for ACK)
	frame[5] = (uint8_t)((DH & 0xFF000000) >> 24); //Serial Number High (MSB Destination Adress)
	frame[6] = (uint8_t)((DH & 0x00FF0000)>>16);
	frame[7] = (uint8_t)((DH & 0x0000FF00)>>8);
	frame[8] = (uint8_t)((DH & 0x000000FF));
	frame[9] = (uint8_t)((DL & 0xFF000000) >> 24); //Serial Number Low (LSB Destination Adress)
	frame[10] = (uint8_t)((DL & 0x00FF0000)>>16);
	frame[11] = (uint8_t)((DL & 0x0000FF00)>>8);
	frame[12] = (uint8_t)((DL & 0x000000FF));
	frame[13] = (uint8_t)((dest_address & 0xFF00)>>8); // 16-bit destination network adress if known.
	frame[14] = (uint8_t)(dest_address & 0x00FF); // Set to 0xFFFE if the address is unknown, or if sending a broadcast.
	frame[15] = broad_radius; 	// Broadcast radius. Sets maximum number of hops a broadcast transmission can take.
								// If set to 0, the broadcast radius will be set to the maximum hops value.
	frame[16] = tx_options;

	for (i=0;i<size;i++) //coloca os dados na trama
	{
		frame[17+i]=((uint8_t)data[i]);
	}

	API.checksum = (0xFF - (uint8_t)(API.frame_type + API.frame_id + sum_of_32bits(DH) + sum_of_32bits(DL) + sum_of_16bits(dest_address) + broad_radius + tx_options + sum_of_char(data, size)) ) ;
	frame[17+i] = (API.checksum);           // Sends CRC to module

	#ifdef DEBUG
		printf("FrameTX= ");
		for (i=0;i<frame_size;i++)
			printf("0x%X ",frame[i]);
		printf("\n");
	#endif

	//SEND FRAME//
	w_serial = serial_write(fd,frame,frame_size);
	#ifdef DEBUG
		if(w_serial < 0)
			printf("Erro acesso porta série");
		if(w_serial<frame_size)
			printf("Escrita incompleta");
	#endif
	free(frame);
	frame=NULL;
	/////////////
	if(TX_status(fd) == 0)
		return true;
	else return false;

return false;
}

uint8_t TX_status(int fd)
{
	uint8_t frame_id, delimiter, tx_retry_count, discovery_status, frame_size=11, r_serial, i;
	uint8_t buffer[frame_size];
	uint16_t dest_address;

	r_serial=serial_read(fd, buffer, frame_size, timeout);
	#ifdef DEBUG
		if(r_serial < 0)
				printf("Erro acesso porta série! \n");
		if(r_serial < frame_size)
				printf("\n Erro por timeout! \n");

		for (i=0; i<frame_size;i++)
			{printf("TX_Status %d: ",i);
			printf("%x \n",buffer[i]);}
	#endif

	delimiter = buffer[0];                             	// Start Delimiter
	if(delimiter != start_delimiter)
	{
		API_status = 0x05;
		return 0xFF;
	}

	API.length = buffer[1];             		    		// Length
	API.length = API.length<<8;
	API.length = API.length | buffer[2];
	API.frame_type = buffer[3];        // API.frame_type
	if((API.frame_type) != (0x8b))
	{
		return 0xFF;
	}
	frame_id = buffer[4];                              	// Frame ID
	if(frame_id != API.frame_id)
	{
		#ifdef DEBUG
			printf("\n[3]\n");
		#endif
		API_status = 0x02;
		return 0xFF;
	}
	API.frame_id = frame_id;

	dest_address = buffer[5];								// 16-bit address of destination
	dest_address = dest_address<<8;
	dest_address = dest_address | buffer[6];

	tx_retry_count = buffer[7];							// TX Retry Count

	delivery_status = buffer[8];                           // Delivery status

	if(delivery_status > 0x75)
	{
		delivery_status = 0xFF;
		return 0xFF;
	}

	discovery_status = buffer[9];

	API.checksum = buffer[10];                         		// Checksum
	if( (uint8_t)(API.checksum + API.frame_type + API.frame_id + sum_of_16bits(dest_address) + tx_retry_count + delivery_status + discovery_status) == 0xFF )
	{
		API_status = 0x00;
	}
	else
	{
		API_status = 0x03; // CRC error
		return 0xFF;
	}

	printf("Delivery Status: %d\n",delivery_status);
return delivery_status;
}

uint8_t* RX_64bit(int fd)
{
	uint8_t r_serial, frame_size=4, data_size, data_size_array, aux=0, i=0, aux1=0;
	unsigned char rx_buffer[4], *rx_buffer1;
	uint8_t *data_array;

	while(aux==0)	//Espera até que algum nó envie uma trama
	{
		r_serial=serial_read(fd, rx_buffer, 4, timeout);
		#ifdef DEBUG
				printf("r_seri<al= %d\n", r_serial);
		#endif
		if(r_serial == frame_size)
			aux=1;
	}
	#ifdef DEBUG
		printf("Imprime os 1os 4bytes: ");
		for (i=0; i<frame_size;i++)
			printf("0x%X ", rx_buffer[i]);
		printf("\n");
		printf("DEPOIS IMPRIMIR 4");
	#endif

	if(rx_buffer[0] != start_delimiter)                    // Start condition check
		API_status = 0x05;                              // Start frame error

	API.length = rx_buffer[1];              			// Length (MSB)
	API.length = API.length<<8;
	API.length = API.length | rx_buffer[2]; 			// Length (LSB)
	API.frame_type = rx_buffer[3];          			// Frame type

	//RECEIVE PACKET//
	if (API.frame_type == 0x90)
	{
		frame_size = API.length; //tamanho do resto da trama que falta receber
		data_size_array = frame_size - 12 + 8 + 1 +1; //tamanho dos dados + Source Address + API.frame_type + data_size
		data_size = frame_size - 12;
		#ifdef DEBUG
			printf("ANTES MALLOC");
		#endif
		rx_buffer1=(unsigned char *)malloc(frame_size*sizeof(unsigned char)); //defino array para o resto da trama
		if (rx_buffer1==NULL) return NULL;
		data_array=(uint8_t *)malloc(data_size_array*sizeof(uint8_t)); //defino array com tamanho dos dados
		if (data_array==NULL) return NULL;
		#ifdef DEBUG
			printf("DEPOIS MALLOC");
		#endif
		aux1=1;

		r_serial=serial_read(fd, rx_buffer1, frame_size, timeout); //lê os valores após frame_type
		#ifdef DEBUG
			printf("r_serial= %d | frame_size= %d\n", r_serial, frame_size);
			printf("Imprime os os restantes bytes: ");
				for (i=0; i<frame_size;i++)
					printf("0x%X ", rx_buffer1[i]);
			printf("\n");
		#endif
		receive_options = rx_buffer1[10];                 // Receive options

		data_array[0]=data_size_array;
		data_array[1]=API.frame_type;

		for(i=0;i<8;i++) //guarda no array os 8bytes do source address
			data_array[i+2]=rx_buffer1[i];

		for(i=0;i<data_size; i++) //guarda no array de dados os dados
		{
			data_array[10+i]=rx_buffer1[11+i];
		}

		API.checksum = rx_buffer1[11+i];            // Checksum
		// Checksum verification
		if( (uint8_t) (API.frame_type + sum_of_char(rx_buffer1, (frame_size-1))) == 0xFF)
		{
			API_status = 0x00;	// Checksum OK
		}
		else
		{
			API_status = 0x03; // Bad CRC
		}
	#ifdef DEBUG
		printf("Data_array: ");
		for(i=0;i<data_size_array; i++) //guarda no array de dados os dados
			printf("0x%X ", data_array[i]);
		printf("\n");
	#endif
	}
	//MODEM_STATUS//
	if (API.frame_type == 0x8A)
	{
		frame_size = API.length; //tamanho do resto da trama que falta receber
		data_size = frame_size+1; //Status(dados) + API.frame_type + data_size
		rx_buffer1=(unsigned char *)malloc(frame_size*sizeof(unsigned char)); //defino array para o resto da trama
		if (rx_buffer1 == NULL) return NULL;
		data_array=(uint8_t *)malloc(data_size*sizeof(uint8_t)); //defino array com tamanho dos dados
		if (data_array == NULL) return NULL;
		aux1=1;

		r_serial=serial_read(fd, rx_buffer1, frame_size, timeout); //lê os valores após frame_type
		#ifdef DEBUG
			printf("r_serial= %d | frame_size= %d\n", r_serial, frame_size);
			printf("Imprime os restantes bytes: ");
				for (i=0; i<frame_size;i++)
					printf("0x%X ", rx_buffer1[i]);
		#endif
		data_array[0]=data_size;
		data_array[1]=API.frame_type;
		data_array[2]=rx_buffer1[0]; //Status

		API.checksum = rx_buffer1[1];            // Checksum
		// Checksum verification
		if( (uint8_t) (API.frame_type + sum_of_char(rx_buffer1, (frame_size-1))) == 0xFF)
		{API_status = 0x00;}	// Checksum OK
		else
		{API_status = 0x03;} // Bad CRC

	#ifdef DEBUG
			printf("Data_array: ");
			for(i=0;i<data_size; i++) //guarda no array de dados os dados
				printf("0x%X ", data_array[i]);
			printf("\n");
		#endif
	}

	if (aux1 == 1) {free(rx_buffer1); rx_buffer1 = NULL;}
	else return NULL;

return data_array;
}

uint8_t TX_AT_cmd(int fd, char *AT_cmd)
{
	uint8_t frame[8], w_serial;
	API.frame_id = API.frame_id+1;
	if(API.frame_id == 0)                // Frame_ID = 0 -> No response
		API.frame_id = 1;
	API.frame_type = 0x08;               // 0x08->AT Command
	API.AT_cmd[0] = AT_cmd[0];
	API.AT_cmd[0] = AT_cmd[1];

	frame[0] = start_delimiter;		// Start delimiter
	frame[1] = 0;					// MSB Length
	frame[2] = 4;			// LSB Length (change this for other type of payload)
	frame[3] = API.frame_type;			// API.frame_type->AT Command
	frame[4] = API.frame_id;			// Frame ID (for ack)
	frame[5] = API.AT_cmd[0];		// First char of AT command
	frame[6] = API.AT_cmd[1];		// Second char of AT command
	API.checksum = (0xFF - (uint8_t)(API.frame_type + API.frame_id + API.AT_cmd[0] + API.AT_cmd[1]));
	frame[7]=API.checksum; // CRC to module

	//SEND FRAME//
	w_serial = serial_write(fd, frame, 8);
	#ifdef DEBUG
		if(w_serial < 0)
			printf("Erro acesso porta série");
		if(w_serial<8)
			printf("Escrita incompleta");
	#endif
	/////////////

return API.frame_id;
}

/*
uint8_t RX_AT_cmd()
{
	uint8_t frame_id, delimiter;

	delimiter = UART_Receive();					// Start delimeter
	if(delimiter !=  start_delimiter)           // Start condition check
	{
		API_status = 0x05;						// Start frame error
		return 1;								// ERROR
	}

	API.length = UART_Receive();               	// Length
	API.length = API.length<<8;
	API.length = API.length | UART_Receive();
	API.frame_type = UART_Receive();           	// API identifier
	frame_id = UART_Receive();
	                 	// Frame ID
	if(frame_id != API.frame_id)            	// Frame ID check
	{
		API_status = 0x02;                      // Frame ID error
		return 1;								// ERROR
	}

	API.AT_cmd[0] = UART_Receive();            	// AT1
	API.AT_cmd[1] = UART_Receive();            	// AT2
	cmd_status = UART_Receive();               	// Status
	if (cmd_status==0)
		return 0;
	else if (cmd_status!=0)
		return 1;
return 0;
}


bool get_parent()
{
	uint16_t cmd_data;
	API.AT_cmd[0]='M';
	API.AT_cmd[1]='P';//'P';
	TX_AT_cmd(4);
	CRC_AT_calc();

	if (RX_AT_cmd() != 0)
		return false;
	//Receives the parameter
	cmd_data = UART_Receive();
	cmd_data = (cmd_data << 8) | UART_Receive();

	API.checksum = UART_Receive();                         		// Checksum
	if((uint8_t)(API.checksum + API.frame_type + API.frame_id + API.AT_cmd[0] + API.AT_cmd[1] + cmd_status + sum_of_16bits(cmd_data) == 0xFF))
	{
		API_status = 0x00;
	}


	if (cmd_data!=0xFFFE)
		return true;
	else return false;
}

uint32_t get_config(uint32_t DL, uint32_t DH)
{
	uint32_t config_data;
	unsigned char data [2];
	data[0]=100;
	data[1]=101;

	if ((TX_req_64bit(DL, DH, &data, 2) == false))
		return false;

	config_data = RX_64bit();

	return config_data;
}
*/


/*REMOTE AT inc.
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
