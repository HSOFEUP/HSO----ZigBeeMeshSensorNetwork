/*
 * main.c
 *
 *  Created on: 16 de Abr de 2012
 *
 */

#include <stdio.h>   //Standard input/output definitions
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <ctime>	// !!Check if include is necessary
#include <cstdio>	// !!Check if include is necessary
#include <time.h>	// !!Check if include is necessary
#include <math.h>
#include "serial_util1.h"
#include "xbee_api_linux.h"
#include "cpp_to_php.h"
#include "debug.h"

#define MODEMDEVICE "/dev/ttyUSB0"
#define MODEMDEVICE_1 "/dev/ttyUSB1"
#define timeout 1
#define MAX_END_DEVICE 9
#define RX_PACKET 0x90
#define MODEM_STATUS 0x8A
#define RX_DATA_ID 10 			// Position of array data_array

/*
 * Data_ID - identifies the data type
 */
#define SENS_LUZ 1
#define SENS_SOM 2
#define NIVEL_BAT 100
#define PEDIDO_CONFIG 101 //=0x65

uint8_t data_tx[128];	// Data to send; when I read CONFIG.time

char nibble_to_char(unsigned int nibble)
{
	if ((nibble >= 0x00) && (nibble <= 0x09))
		return (nibble + 0x30);

	else if ((nibble >= 0x0A) && (nibble <= 0x0F))
		return (nibble - 10 + 0x41);

	else
		return 0x00;
}

string Bytes_to_str(unsigned char *input_byte, unsigned int init, unsigned int size)
{
	string s_out;
	unsigned int high_nibble, low_nibble;
	unsigned int i=init;

	for (i=init; i < (init + size -1); i++)
	{
		high_nibble = (input_byte[i] & 0xF0) >> 4;
		s_out += nibble_to_char(high_nibble);

		low_nibble = (input_byte[i] & 0x0F);
		s_out += nibble_to_char(low_nibble);
	}
return s_out;
}

uint32_t create32b (uint8_t *address, uint8_t start, uint8_t size) // Function that returns 4 LSB da RX_PACKET
{
	uint32_t address_32=0;
	int i=0;
	for (i=start; i<(start+size-1);i++)
	{
		address_32 += address[i];
		address_32 = address_32<<8;
	}
		address_32 += address[i++];
return address_32;
}

uint16_t str_to_int(string str)
{
	std::string myString = str;
return (atoi(myString.c_str()));
}

string int_to_str(float_t number)
{
   stringstream ss;	//create a stringstream
   ss << number;	//add number to the stream
   return ss.str();	//return a string with the contents of the stream
}

void data_to_send_16b(uint16_t data, uint8_t start)
{
	data_tx[start]=(uint8_t)((data & 0xFF00)>>8);
	data_tx[start+1]=(uint8_t)(data & 0x00FF);
}

uint16_t uchar_to_int(uint8_t *data, uint8_t start)
{
	uint16_t result;
	result = data[start];
	result = (result << 8);
	result = result|data[start+1];
return result;
}

float_t calc_spl_sensor(uint16_t adc_spl)
{
	float_t result;
	result=9.4374*log(adc_spl)+29.84;
return result;
}

string get_datetime()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	strftime (buffer,80,"%Y-%m-%dT%H:%M:%S.000Z",timeinfo);
return buffer;
}

/*
 * MAIN PROGRAM
 */
int main(void)
{
	int fd=-1, pos_node_id=-1;
	bool TX_res;
	uint8_t *data_array; 			// *data_array-> data from Receive Packet
	uint16_t time_node, value_16b; 	//time_node: Limit to 0-65535 seconds = 18,2h
	string store_id, node_id, sensor_id_light, sensor_id_sound, read_datetime, read_value_lux, read_value_visible_light, read_value_infra_red_light, read_value_dba;
	uint32_t address_dest[2];

	while(fd==-1) // Wait until the xbee will be connected
	{
		fd=serial_open(MODEMDEVICE);	// Open Serial Port
		if (fd == -1)					// Check if Serial Port is open
			fd=serial_open(MODEMDEVICE_1);
		if (fd == -1)
			printf("Cannot open\n");
		else printf("Can open: %d\n", fd);
	}

	while (config_db(0)==-1); // Check first configurations of nodes

	while (1)
	{
		#ifdef DEBUG
				cout<<"Waiting for data from sensors"<<endl;
		#endif
		data_array=RX_64bit(fd);
		//RX-PACKET//
		if (data_array[1]==RX_PACKET && data_array!=NULL)
		{
			node_id=Bytes_to_str(data_array, 2, 9);
			pos_node_id=search_node_id(node_id);
			address_dest[0]=create32b(data_array, 2, 4);	//32MSB Source address
			address_dest[1]=create32b(data_array, 6, 4);	//32LSB Source address
			/////////PEDIDO DE CONFIGURACAO/////////
			if ((data_array[RX_DATA_ID]==PEDIDO_CONFIG) && (pos_node_id!=-1)) //Verifica se é um pedido de configuracao e se tem configuracao
			{
				#ifdef DEBUG
					cout<<"Configuration Request"<<endl;
				#endif
				config_db(-1);
				time_node = str_to_int(get_time_node(pos_node_id)); // Convert string to uint16_t
				data_to_send_16b(time_node, 0);
				TX_res=TX_req_64bit(fd, address_dest[1], address_dest[0], data_tx, 2);
				#ifdef DEBUG
					if (TX_res==true)
						printf("TX success - configuration sent");
					else printf("TX NOT success - configuration not sent");
				#endif
			}
			/////////DADOS DOS SENSORES/////////
			else if ((data_array[RX_DATA_ID]>0) && (data_array[RX_DATA_ID]<100) && (pos_node_id!=-1)) // Verifica se são leituras dos sensores
			{
				cout<<"DADOS DOS SENSORES"<<endl;
				sensor_id_light = int_to_str(data_array[10]);
				value_16b = uchar_to_int(data_array, 11); // Visible Light
				read_value_visible_light = int_to_str(value_16b);
				value_16b = uchar_to_int(data_array, 13); // Infrared Light
				read_value_infra_red_light = int_to_str(value_16b);
				value_16b = uchar_to_int(data_array, 15); // Lux Light
				read_value_lux = int_to_str(value_16b);
				//Sensor ID - Sound
				sensor_id_sound = int_to_str(data_array[17]);
				value_16b = uchar_to_int(data_array, 18); // SPL
				read_value_dba = int_to_str(calc_spl_sensor(value_16b));
				//read_value_dba = int_to_str(value_16b);
				// Datetime
				read_datetime = get_datetime();
				// Store ID
				store_id="21";//get_store_id(0);
				// Checks for new configuration
				if(config_db(-1)==0)
				{
					//transmits new configuration
					time_node = str_to_int(get_time_node(pos_node_id)); // Convert string to uint16_t
					data_to_send_16b(time_node, 1);
					data_tx[0] = 103;
					TX_res=TX_req_64bit(fd, address_dest[1], address_dest[0], data_tx, 3);
					if(TX_res==true)
						printf("TX success - new configuration");
					else printf("TX NOT success - new configuration");
				}
				else
				{
					data_tx[0]=104;
					TX_res=TX_req_64bit(fd, address_dest[1], address_dest[0], data_tx, 1);
					if(TX_res==true)
						printf("TX success\n");
					else printf("TX NOT success");
				}
				//SEND DATA TO PHP
				//light
				//config_db(1);
				cpp_to_php(store_id, node_id, sensor_id_light, read_datetime, read_value_lux, read_value_visible_light, read_value_infra_red_light, "0");
				//sound
				cpp_to_php(store_id, node_id, sensor_id_sound, read_datetime, "0", "0", "0", read_value_dba);
				printf("SAI DADOS\n");
			}
		}
	free(data_array);
	data_array = NULL;
	}
	#ifdef DEBUG
		cout << "END!" << endl;
	#endif
return 1;
}
