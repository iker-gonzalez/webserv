// Webserver_Zaloa.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include "../includes/ConfigFile.hpp"

int main(int ac, char **av)
{
	std::string filename;
	if (ac == 1)
		filename = "config_files/Conf_file.conf";
	else
		filename = av[1];

	//Bueno
	ConfigFile conf(filename);

	if (!conf.CheckConfig())
	{
		std::cout << "Bad ConfigFile" << std::endl;
	}
	
	return 0;
}
