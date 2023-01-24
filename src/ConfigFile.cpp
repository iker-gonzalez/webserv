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
		return false;
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
		std::cout << "Infile unable to open" << std::endl;
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
				int nbr_location = _v_server.at(_v_server.size() - 1).getLocation().size();
				Location& tmp_location = _v_server.at(_v_server.size() - 1).getLocationByReference(nbr_location - 1);
				if (!tmp_location.Parse(line))
					return false;
				if (tmp_location.isCloseBracket())
					_v_server.at(_v_server.size() - 1).locationBracketOpen = false;
			}
			else if (!_v_server.at(_v_server.size() - 1).Parse(line))
				return false;
		}

		
		std::cout << line << std::endl;
	}
	for (unsigned int i = 0; i < _v_server.size(); i++)
	{
		std::cout << _v_server[i] << std::endl;
	}
	return true;
}

bool ConfigFile::ParseFile(std::string &aline) const
{
	return false;
}
