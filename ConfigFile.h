#pragma once

#include <string>
#include <vector>
class Server;
class ConfigFile
{
	public:
		ConfigFile(std::string& afilename);
		~ConfigFile();
	
		//! return true if configuration file is okey, otherwise false
		bool CheckConfig(void) ;
	
		//! Check extension of the configuration file 
		bool CheckName(void) const;
	
		//! Check if exist and reading permissions
		bool ReadFile(void) ;
	
		//! Check correct configuration file format
		bool ParseFile(std::string& aline) const;
	
	private:
	
		//! Name of the configuration file
		const std::string _filename;
	
		//! Servers vectors
		std::vector<Server> _v_server;
};

