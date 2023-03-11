// Webserver_Zaloa.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include "../includes/ConfigFile.hpp"
#include "../includes/Utils.hpp"
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"
#include "../includes/ServerManager.hpp"

int main(int ac, char **av)
{
	// Get the configuration file, if there is not one given as argument take default.conf 
	std::string filename;
	if (ac == 1)
		filename = "config_files/Conf_file.conf";
	else
		filename = av[1];

	// Check configuration file and get all information
	ConfigFile conf(filename);
	if (!conf.CheckConfig())
	{
		std::cout << "Bad ConfigFile" << std::endl;
		return false;
	}

	try {
		conf.CheckNecessaryInfo();
	}
	catch(std::string error)
	{
		std::cerr << error << std::endl;
	}
	
	//! TODO Create Select Core
	ServerManager manager(conf.getServers());
	if (!manager.setupServers()) 
		return false;
	std::cout <<  "Start listening for incoming connections" << std::endl;

	if (!manager.serverCore())
	{
		manager.closeServerSocket();
		return false;
	}
	return 0;
}
