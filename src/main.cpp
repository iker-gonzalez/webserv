// Webserver_Zaloa.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include "../includes/ConfigFile.hpp"
#include "../includes/Utils.hpp"
#include "../includes/Request.hpp"

int main(int ac, char **av)
{
	// Get the configuration file, if there is not one given as argument take default.conf 
	std::string filename;
	if (ac == 1)
		filename = "config_files/Conf_file.conf";
	else
		filename = av[1];

	// Check configuration file fand get all information
	ConfigFile conf(filename);
	if (!conf.CheckConfig())
	{
		std::cout << "Bad ConfigFile" << std::endl;
	}
	
	//! TODO Create Select Core
	//if (non_blocking_server(conf))
	//	std::cout << "Error Selectcd" << std::endl;


	// Test the request parse
	Request requestObject;
	std::string request = "GET / HTTP/1.1\r\nHost: localhost:4242\r\nConnection: keep-alive\r\nsec-ch-ua: \"Not_A Brand\";v=\"99\", \"Google Chrome\";v=\"109\", \"Chromium\";v=\"109\"\r\nsec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: \"macOS\"\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,/;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: es-ES,es;q=0.9";
	requestObject.requestParsing(request);

	//! TODO Test response message

	return 0;
}
