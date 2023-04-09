#include "../includes/ConfigFile.hpp"

#include <fstream> // for ifstream
#include <iostream>
#include "../includes/Server.hpp"
#include "../includes/Location.hpp"
#include "../includes/Utils.hpp"

ConfigFile::ConfigFile(std::string& afilename) : _filename(afilename)
{

}

ConfigFile::~ConfigFile()
{
}

bool ConfigFile::CheckConfig() 
{
	if (!CheckExtension(_filename, std::string(".conf")))
	{
		std::cerr << "Bad extension" << std::endl;
		return false;
	}
	if (!ReadFile())
		return false;
	return true;
}


void replace_file(std::string& line, std::string s1, std::string s2)
{
	unsigned int pos = 0;
	while (std::string::npos != line.find(s1))
	{
		pos = line.find(s1);
		line.erase(pos, s1.length());
		line.insert(pos, s2);
	}
}
bool ConfigFile::ReadFile(void) 
{
	std::ifstream infile(_filename.c_str());
	if (!infile.is_open())
	{
		std::cerr << "Infile unable to open" << std::endl;
		return (false);
	}
	std::string line;
	while (std::getline(infile, line))
	{
		if (!line.empty())
			replace_file(line, std::string("\t"), std::string(" "));
		if (line.empty())
			continue;
		if (!line.compare(0, 6, "server"))
		{
			if (_v_server.size() && _v_server.at(_v_server.size() - 1).locationBracketOpen)
				return false;
			Server server;
			if (!server.Parse(line))
				return false;
			_v_server.push_back(server);
		}
		else if (!line.compare(1, 8, "location"))
		{
			if (_v_server.at(_v_server.size() - 1).locationBracketOpen)
				return false;
			Location location;
			if (!location.Parse(line))
				return false;
			_v_server.at(_v_server.size() - 1).pushBackLocation(location);
			_v_server.at(_v_server.size() - 1).locationBracketOpen = true;

		}
		else
		{
			if (_v_server.at(_v_server.size() - 1).locationBracketOpen)
			{
				int nbr_location = _v_server.at(_v_server.size() - 1).getLocations().size();
				Location& tmp_location = _v_server.at(_v_server.size() - 1).getLocationsByReference(nbr_location - 1);
//				std::cout << "tem_loca: " << tmp_location << std::endl; 
				if (!tmp_location.Parse(line))
					return false;
				if (tmp_location.isCloseBracket())
					_v_server.at(_v_server.size() - 1).locationBracketOpen = false;
			}
			else if (!_v_server.at(_v_server.size() - 1).Parse(line))
				return false;
		}

		
		//std::cout << line << std::endl;
	}
	for (unsigned int i = 0; i < _v_server.size(); i++)
	{
		//std::cout << _v_server[i] << std::endl;
	}
	return true;
}

bool ConfigFile::ParseFile(std::string &aline) const
{
	return false;
}


// Neccesarry data of the confif file:
// 1. Address.
// 2. Port.
// 3. Index.
// Other information has a default value.
void ConfigFile::CheckNecessaryInfo() const
{
	std::vector<Server>::const_iterator it_server;
	std::vector<Server>::const_iterator it_server_end = _v_server.end();
	for (it_server =  _v_server.begin(); it_server != it_server_end; it_server++)
	{
		if (!(*it_server).isPort())
			throw "Lack of necessary server info: (Port)";
		if ( !(*it_server).isAddress())
			throw "Lack of necessary server info: (Port or Address)";
		if (!(*it_server).isServerName())
			throw "Lack of necessary server info: (Port or Address)"; std::vector<Location> v_location  = (*it_server).getLocations();
		std::vector<Location>::const_iterator it_location;
		std::vector<Location>::const_iterator it_location_end = v_location.end();
		for (it_location = v_location.begin(); it_location != it_location_end; it_location ++ )
		{
			if (!(*it_server).isRoot() && !(*it_location).isRoot())
				throw std::string("Lack of necessary server info: (Root)");
			if (!(*it_server).isIndex() && !(*it_location).isIndex())
				throw std::string("Lack of necessary server info: (Index)");
		}
	}

	return ;
}

std::vector<Server> ConfigFile::getServers()const
{
	return _v_server;
}