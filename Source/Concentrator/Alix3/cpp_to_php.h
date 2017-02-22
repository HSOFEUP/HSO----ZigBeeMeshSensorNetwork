/*
 * cpp_to_php.h
 *
 *  Created on: 28 de Mai de 2012
 *
 */

#ifndef CPP_TO_PHP_H_
#define CPP_TO_PHP_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sstream>
#include <iostream>
#include <string>
using namespace std;
#include "debug.h"

#define BASE_ADDRESS "127.0.0.1"
#define SUB_REQUEST "/data_to_webservice.php"  //Send configurations for remote Web Service
#define SUB_REQUEST_CFG_1 "/config_1.php" //Configurations on first time: id_mac and time
#define SUB_REQUEST_CFG "/config.php" //id_mac and time
#define SUB_REQUEST_WCF "/config_webservice.php" //store_id and wcf
#define SERVER_PORT 80

struct
{
	string id_node[20];
	string time [20];
	string id_store[5];
	string web_service[5];
} CONFIG;

//int cpp_to_php();
int cpp_to_php(string store_id, string node_id, string sensor_id, string read_datetime, string read_value_lux, string read_value_visible_light, string read_value_infra_red_light, string read_value_dba);
int config_db(int type_configuration);
int search_node_id(string sub);
string get_time_node(int pos);
string get_store_id(int pos);
string get_web_service (int pos);

#endif /* CPP_TO_PHP_H_ */
