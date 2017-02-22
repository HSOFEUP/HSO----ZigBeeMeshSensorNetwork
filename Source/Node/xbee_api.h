#include <stdbool.h>
#include <avr/io.h>
#include <stdint.h>

uint16_t sum_of_16bits(uint16_t data);
uint16_t sum_of_32bits(uint32_t data);
uint16_t sum_of_char(char *data, uint8_t size);
void CRC_AT_calc();
uint8_t TX_req_64bit(uint32_t DL, uint32_t DH, uint8_t *data, uint8_t size);
uint8_t TX_status();
uint8_t RX_64bit(); //Retorna o tamanho dos dados
uint8_t TX_AT_cmd(uint16_t length);
uint8_t RX_AT_cmd();

//uint8_t set__sleep_mode();
uint8_t get_parent();
uint8_t ack_new_config(uint16_t *sleep_time_aux);
uint8_t get_config(uint32_t DL, uint32_t DH, uint16_t *sleep_time); 
//Retorna se tem nova configuracaoo ou mantem a default (=0 mantem a default; =1 nova configuracao)
