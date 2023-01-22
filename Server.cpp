#include "Server.h"

Server::Server() : CommonInfo()
{
}


Server::~Server()
{
}


void Server::fillNecssaryInfo()
{

	_m_necessary_info.insert(std::pair<std::string, int>("listen", 2));
	_m_necessary_info.insert(std::pair<std::string, int>("server_name", 2));
	_m_necessary_info.insert(std::pair<std::string, int>("root", 2));
	_m_necessary_info.insert(std::pair<std::string, int>("methods", 4));
	_m_necessary_info.insert(std::pair<std::string, int>("error_page", 2));
	
}
bool Server::fillSpecificInfo(std::vector<std::string>& a_v_strSplit)
{
	if (!(a_v_strSplit[0].compare("server")))
	{
		if (!checkSize(a_v_strSplit, 2, 2) || (a_v_strSplit[1].compare("{")))
			return false;
		_closeBracket = false;
		return true;
	}
	return false;
}
std::vector<Location> Server::getLocation() const
{
	return _v_location;
}
Location& Server::getLocationByReference(int nbr_localitaion) 
{
	return (_v_location.at(nbr_localitaion));
}
void Server::pushBackLocation(Location& a_location)
{
	_v_location.push_back(a_location);
}
/*
bool Server::checkSize(std::vector<std::string> &a_v_info, unsigned int minValue, unsigned int  maxValue)
{
	if (a_v_info.size() < minValue || a_v_info.size() > maxValue)
		return false;

	return true;

}*/

/*bool Server::fillInformation(std::vector<std::string>& a_v_strSplit)
{
	if (!(a_v_strSplit[0].compare("server")) && !(a_v_strSplit[1].compare("{")))
		return true;
	else if (!(a_v_strSplit[0].compare("listen")) && checkSize(a_v_strSplit, 2, 2))
		_listen = a_v_strSplit[1];
	else if (!(a_v_strSplit[0].compare("server_name")) && checkSize(a_v_strSplit, 2, 2))
		_server_name = a_v_strSplit[1];
	else if (!(a_v_strSplit[0].compare("root")) && checkSize(a_v_strSplit, 2, 2))
		_root = a_v_strSplit[1];
	else if (!(a_v_strSplit[0].compare("index")) && checkSize(a_v_strSplit, 2, 2))
		_index = a_v_strSplit[1];
	else if (!(a_v_strSplit[0].compare("methods")) && checkSize(a_v_strSplit, 2, 4))
	{
		a_v_strSplit.erase(a_v_strSplit.begin());
		_v_methods = a_v_strSplit;
	}
	else if (!(a_v_strSplit[0].compare("error_page")) && checkSize(a_v_strSplit, 2, 3))
	{
		_m_error_page.insert(std::pair<int, std::string>(std::stoi(a_v_strSplit[1]), a_v_strSplit[2]));
	}
	else if (!(a_v_strSplit[0].compare("client_size")) && checkSize(a_v_strSplit, 2, 2))
		_client_size = std::stoi(a_v_strSplit[1]);
	else if (!(a_v_strSplit[0].compare("autoindex")) && checkSize(a_v_strSplit, 2, 2))
		_autoindex = true;
	else if (!(a_v_strSplit[0].compare("location")) && checkSize(a_v_strSplit, 2, 3))
		(void)a_v_strSplit;
	else if (!(a_v_strSplit[0].compare("cgi_pass")) && checkSize(a_v_strSplit, 2, 2))
		_cgi_pass = a_v_strSplit[1];
	else if (!(a_v_strSplit[0].compare("}")) && !isCloseBracket())
		_closeBracket = true;
	else
		return false;
	return true;

}
*/
/*
void replace_file(std::string& line, std::string s1, std::string s2)
{
	unsigned int pos = 0;
	while (std::string::npos != line.find(s1))
	{
		pos = line.find(s1);
		line.erase(pos, s1.length());
		line.insert(pos, s2);
	}
}*/
//1. Split de saltos de linea
//2. Bucle y split de espacios
//3. if else de la información
/*bool Server::Parse(std::string aLine)
{
	std::string line_spaces;
	if (aLine.empty())
		return true;
	std::string a = "\t";
	std::string b = " ";
	replace_file(aLine, a, b);
	std::vector<std::string> v_strSplit;
	std::string s = " ";
	if (!splitString(aLine, s, v_strSplit))
		return false;
	if (!fillInformation(v_strSplit))
		return false;
	return true;


}
*/

std::ostream& operator<<(std::ostream& ors, const Server& server)
{
	ors << "----SERVER----" << std::endl;
	ors << "listen" << server.getListen() << std::endl;
	ors << "server_name" << server.getServerName() << std::endl;
	ors << "root" << server.getRoot() << std::endl;
	ors << "index" << server.getIndex() << std::endl;
	std::vector<std::string> v_methods = server.getMethods();
	for (unsigned int i = 0; i < v_methods.size(); i++) {
		ors << "methods[" << i << "]: " << v_methods[i] << std::endl;
	}
	std::map<int, std::string> m_errors = server.getErrorPage();
	std::map<int, std::string>::iterator it;
	for (it = m_errors.begin(); it != m_errors.end(); it++){
		ors << "error_page: "
			<< it->first    // string (key)
			<< ':'
			<< it->second   // string's value 
			<< std::endl;
	}
	ors << "client_size" << server.getClientSize() << std::endl;
	ors << "autoindex" << server.getAutoindex() << std::endl;
	std::vector<Location> v_locations = server.getLocation();
	for (unsigned int i = 0; i < v_locations.size(); i++)
	{
		ors << "  ---Location [" << i << "]: " << v_locations[i] << std::endl;
	}
	return ors;
}
