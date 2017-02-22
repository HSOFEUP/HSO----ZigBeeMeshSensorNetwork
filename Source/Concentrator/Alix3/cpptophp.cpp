/*
 * cpptophp.cpp
 *
 *  Created on: 28 de Mai de 2012
 *
 */
#include "cpp_to_php.h"

int position_vect_config=0;

/*
 * Variable:
 * type_configuration=0 --> first config, only updates the time
 * type_configuration=-1 --> not first config, only updates the time
 * type_configuration= 1 --> only updates the Web Service Address
 * //int first_config=0; //(0)-First time; (-1)-Not first time
 * */

int cpp_to_php(string store_id, string node_id, string sensor_id, string read_datetime, string read_value_lux, string read_value_visible_light, string read_value_infra_red_light, string read_value_dba)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

	//string store_id1 = "3";
	//string node_id = "3";
	//string sensor_id = "3";
	//string read_datetime = "2007-11-28T16:00:00.000Z";
	//string read_value_lux = "3";
	//string read_value_visible_light = "3";
	//string read_value_infra_red_light = "3";
	//string read_value_dba = "3";
    string web_service ="a";
    //string web_service = "http://seeplus-demo.cloudapp.net/Services/Environment/I9REnvironment.svc?wsdl";
    //string web_service =  "http://seeplusgng.cloudapp.net/Services/Environment/I9REnvironment.svc?wsdl";

	string url = BASE_ADDRESS;
	string params = "?store_id=" + store_id;
	params += "&node_id=" + node_id;
	params += "&sensor_id=" + sensor_id;
	params += "&read_datetime=" + read_datetime;
	params += "&read_value_lux=" + read_value_lux;
	params += "&read_value_visible_light=" + read_value_visible_light;
	params += "&read_value_infra_red_light=" + read_value_infra_red_light;
	params += "&read_value_dba=" + read_value_dba;
	params += "&web_service=" + web_service;

    char buffer[4096];

    portno = SERVER_PORT;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
		#ifdef DEBUG
			cout << "ERROR opening socket" << endl;
		#endif
    }

	#ifdef DEBUG
		cout << "Trying to connect to: " << url << endl;
	#endif

    server = gethostbyname(url.c_str());
    if (server == NULL)
	{
		#ifdef DEBUG
			cout << "ERROR, no such host" << endl;
			cout << "Decent error description is: " << hstrerror(h_errno) << endl;
		#endif
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
		#ifdef DEBUG
			cout << "ERROR connecting" << endl;
		#endif
    }

	#ifdef DEBUG
		cout << "Connected" << endl;
	#endif

	string request = "GET ";
	request += SUB_REQUEST;
	request += params;
	request += " HTTP2.0\n\r\n\r";

	#ifdef DEBUG
		cout << "Asking for page " << request << endl;
	#endif

    n = write(sockfd,request.c_str(),request.size());
    if (n < 0)
    {
		#ifdef DEBUG
			cout << "ERROR writing to socket" << endl;
		#endif
    }

	#ifdef DEBUG
		cout << "All ok writing to server, trying to read response!" << endl;
	#endif

	n = 1;
	string response = "";

	while (n != 0)
	{
		bzero(buffer,4096);
		n = read(sockfd, buffer, 4096);
		response += buffer;

		if (n < 0)
		{
			#ifdef DEBUG
				cout << "ERROR reading from socket" << endl;
			#endif
		}
	}

	#ifdef DEBUG
		cout << "I have read " << n << " characters!" << endl;
	    cout << response << endl;
	#endif

    close(sockfd);
    return 0;
}

int config_db(int type_configuration)
{
    int sockfd, portno, n, aux=0, aux_n=0, res_search_node=-1;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    string url = BASE_ADDRESS;

    char buffer[4096];

    portno = SERVER_PORT;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
		#ifdef DEBUG
			cout << "ERROR opening socket" << endl;
		#endif
    }
	#ifdef DEBUG
		cout << "Trying to connect to: " << url << endl;
	#endif

    server = gethostbyname(url.c_str());
    if (server == NULL)
	{
		#ifdef DEBUG
			cout << "ERROR, no such host" << endl;
			cout << "Decent error description is: " << hstrerror(h_errno) << endl;
		#endif
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
		#ifdef DEBUG
			cout << "ERROR connecting" << endl;
		#endif

	#ifdef DEBUG
		cout << "Connected" << endl;
	#endif

	string request = "GET ";

	if(type_configuration==0)
	{
		request += SUB_REQUEST_CFG_1;
	}
	else if (type_configuration==-1)
			request += SUB_REQUEST_CFG;
	else if (type_configuration==1)
			request += SUB_REQUEST_WCF;
	request += " HTTP2.0\n\r\n\r";

	#ifdef DEBUG
		cout << "Asking for page " << request << endl;
	#endif

    n = write(sockfd,request.c_str(),request.size());
    if (n < 0)
		#ifdef DEBUG
			cout << "ERROR writing to socket" << endl;
		#endif

	#ifdef DEBUG
		cout << "All ok writing to server, trying to read response!" << endl;
	#endif

	n = 1;
	string response = "";

	while (n != 0)
	{
		bzero(buffer,4096);
		n = read(sockfd, buffer, 4096);
		response += buffer;

		if (n < 0)
			#ifdef DEBUG
				cout << "ERROR reading from socket" << endl;
			#endif
	}

	//Parse the string to fetch parameters

	#ifdef DEBUG
		cout << response << endl;
	#endif

if ((type_configuration==0) || (type_configuration==-1))
{
    istringstream iss(response);
        do
        {
        	string sub;
            iss >> sub;
			#ifdef DEBUG
				cout << "Substring: " << sub << endl;
			#endif
            if (aux==1)
            	aux_n++;
            if (aux_n==2)
            {
            	res_search_node=search_node_id(sub);
            	if(res_search_node==-1)
            	{
            		position_vect_config++;
            		CONFIG.id_node[position_vect_config-1]=sub;
            	}
            }
            if (aux_n==3)
			{
            	if(res_search_node==-1)
            		CONFIG.time[position_vect_config-1]=sub;
            	else CONFIG.time[res_search_node]=sub;
            	aux_n=1;
			}
            if (sub=="#")
            	aux=1;
        } while (iss);

    for (int i=0; i<position_vect_config;i++)
    {
    	cout << CONFIG.id_node[i] << " " << CONFIG.time[i] << endl;
    }
    if (position_vect_config!=0)
    	type_configuration=-1;
}

if (type_configuration==1)
{
	istringstream iss(response);
	        do
	        {
	        	string sub;
	            iss >> sub;
	            if (aux==1)
	            	aux_n++;
	            if (aux_n==2)
	            {
	            	CONFIG.web_service[0]=sub;
	            }
	            if (aux_n==3)
				{
	            	CONFIG.id_store[0]=sub;
					aux_n=1;
				}
	            if (sub=="#")
	            	aux=1;
	        } while (iss);
		#ifdef DEBUG
	        cout << "IMPRIME WEB SERVICE AND ID_STORE" << endl;
	        cout << "Web service=" << CONFIG.web_service[0] << " ID Store=" << CONFIG.id_store[0] << endl;
		#endif
}

    close(sockfd);

    if (aux != 0) // If something changed return 0
    	return 0;
return -1;
}

int search_node_id(string sub)
{
	int i=0;

	for (i=0; i<20; i++)
	{
		if (CONFIG.id_node[i]==sub)
			return i;
	}
return -1;
}

string get_time_node(int pos)
{
	return CONFIG.time[pos];
}

string get_store_id(int pos)
{
	return CONFIG.id_store[pos];
}

string get_web_service (int pos)
{
	return CONFIG.web_service[pos];
}
