#include "../includes/Server.hpp"

Server::Server() : CommonInfo(), locationBracketOpen(false)
{
	 
}


Server::~Server()
{
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
