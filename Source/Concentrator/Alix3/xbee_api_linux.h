/*
 * xbee_api_linux.h
 *
 *  Created on: 17 de Abr de 2012
 *
 */

#ifndef XBEE_API_LINUX_H_
#define XBEE_API_LINUX_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "serial_util1.h"
#include "debug.h"

#endif /* XBEE_API_LINUX_H_ */

uint16_t sum_of_16bits(uint16_t data);
uint16_t sum_of_32bits(uint32_t data);
uint16_t sum_of_char(uint8_t *data, uint8_t size);
//void CRC_AT_calc();
bool TX_req_64bit(int fd, uint32_t DL, uint32_t DH, uint8_t *data, uint8_t size);
uint8_t TX_status(int fd);
uint8_t* RX_64bit(int fd);
uint8_t TX_AT_cmd(int fd, char *AT_cmd);
//uint8_t RX_AT_cmd();

//bool get_parent();
//uint32_t get_config(uint32_t DL, uint32_t DH);
