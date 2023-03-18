#include "../includes/Server.hpp"
#include <iostream> 				// for std::cerr
#include <sys/socket.h> 			// for socket
#include <fcntl.h>					// for fcntl
#include <arpa/inet.h>				// for htons
#include <netinet/in.h>				// for sockaddr_in
#include <netinet/tcp.h>			// for TCP_MAXSEG macro
#include <cstdlib>					// for std::atoi 
#include "../includes/Server.hpp"
#include <cstring>
#include <cstdio>

Server::Server() : CommonInfo(), locationBracketOpen(false)
{
	_is_address = false;
	_is_port = false;
	_is_server_name = false;

}


Server::~Server()
{
}

Server& Server::operator=(const Server& rhs) 
{
	if (this != &rhs)
	{
		//Copy commun info
		CommonInfo::operator=(rhs);

		//Copy Server info
		this->_port = rhs._port;
		this->_address = rhs._address;
		this->_v_location = rhs._v_location;
		this->_server_name = rhs._server_name;
		this->_listen_fd = rhs._listen_fd;

		//Copy server check info
		_is_address = rhs._is_address;
		_is_server_name = rhs._is_server_name;
		_is_port = rhs._is_port;

		this->locationBracketOpen = rhs.locationBracketOpen;

	}
	return (*this);
}
Server::Server(const Server& other) : CommonInfo(other)
{
	if (this != &other)
	{
		*this = other;
	}

}
bool Server::fillSpecificInfo(std::vector<std::string> & a_v_strSplit)
{
	if (!(a_v_strSplit[0].compare("server")))
	{
		if (!checkSize(a_v_strSplit, 2, 2) || (a_v_strSplit[1].compare("{")))
			return false;
		_closeBracket = false;
		return true;
	}
	else if (!(a_v_strSplit[0].compare("server_name")) && checkSize(a_v_strSplit, 2, 2) &&
		checkServerName(a_v_strSplit[1]))
	{
		_server_name = a_v_strSplit[1];
		return true;
	}
	else if (!(a_v_strSplit[0].compare("listen")) && checkSize(a_v_strSplit, 2, 2) &&
		checkListen(a_v_strSplit[1]))
	{
		_listen = a_v_strSplit[1];
		return true;
	}
	return false;
}

bool Server::listenConnections()
{
	if (listen(_listen_fd, SOMAXCONN) < 0) 
	{
		std::cerr << "Error listening on socket" << std::endl;
		return false;
	}
	if (fcntl(_listen_fd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "Error fcntl on socket" << std::endl;
		return false;
	}
	return true;
}
//1. Create Socket
//2. Set the socket to non-bloking mode
//3. Bind tje socket to an addres and port
bool Server::setupServer(void)
{
	// Create a socket
	/*
		The program starts by creating a socket using the socket function,
		it specifies the address family (AF_INET), the type of socket (SOCK_STREAM)
		and the protocol (0) as parameters.
	*/
	_listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_listen_fd < 0) 
	{
		std::cerr << "Error creating socket" << std::endl;
		return false;
	}

	int max_payload = getMaxPayloadSize(_listen_fd);
	// Set the socket to non-blocking mode
	/*
		The program sets the socket to non-blocking mode using the fcntl function,
		it retrieves the current flags using F_GETFL and then sets the O_NONBLOCK flag using F_SETFL
	*/
	//! Opcion 1
	 int option_value = 1;
	setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int));
	
	//! Opcion 2
//	int flags = fcntl(_listen_fd, F_GETFL, 0);
//	fcntl(_listen_fd, F_SETFL, flags | O_NONBLOCK);
//	fcntl(_listen_fd, F_SETFL, O_NONBLOCK) ;
//	

	// Bind the socket to an address and port
	/*
		The program binds the socket to an address and port using the bind function,
		it creates a sockaddr_in struct and sets the address family, IP address and port number.
	*/

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	inet_pton(AF_INET, _address.c_str(), &(addr.sin_addr)); //? 0)

	//? 1)addr.sin_addr.s_addr = inet_addr(_address.data());

	//? 2) if (inet_aton(_address.c_str(), &addr.sin_addr) == 0) 
	//?{
	//?	std::cerr << "Invalid address: " << _address << std::endl;
	//?	return false;
	//?}
	
	//char buf[INET_ADDRSTRLEN];
	//std::string host_string = "127.0.0.1";
	//inet_ntop(AF_INET, &host, buf, INET_ADDRSTRLEN);
	//std::cout << "listen" << _listen_fd << std::endl;
	int status_bind = bind(_listen_fd, (struct sockaddr *)&addr, sizeof(addr));;
	if (status_bind < 0) 
	{
		std::cerr << "Error binding socket:" << status_bind <<  std::endl;
		perror("bind");
		return false;
	}
	std::cerr << "Bind status: "<<  status_bind <<  std::endl;
	
	return true;
}

int Server::getMaxPayloadSize(int sock) 
{
	int tcp_maxseg = TCP_MAXSEG;
	int max_payload_size;
	socklen_t len = sizeof(max_payload_size);
	int ret = getsockopt(sock, IPPROTO_TCP, tcp_maxseg, &max_payload_size, &len);
	if (ret < 0) {
		std::cout << "ERRROR GARRRAFAL\n";
		// error handling
		return -1;
	}
	std::cout << "MAX_PAYLOAD_SIZE: " << max_payload_size << std::endl;
	return max_payload_size;
}

bool Server::createSocket(void)
{
	return false;
}
bool Server::nonBlokingSocket(void)
{
	return false;
}
bool Server::bindSocket(void)
{
	return false;
}
int Server::getPort() const
{
	return _port;
}
int Server::getListenFd() const
{
	return _listen_fd;
}
std::string Server::getAddress() const
{
	return _address;
}
std::vector<Location> Server::getLocations() const
{
	return _v_location;
}
std::string Server::getServerName() const
{
	return _server_name;
}

Location& Server::getLocationsByReference(int nbr_localitaion)
{
	return (_v_location.at(nbr_localitaion));
}
void Server::setServerName(const std::string& aserver_name)
{
	_server_name = aserver_name;
}
bool Server::isPort() const
{
	if (_is_port)
		return true;
	return false;
}
bool Server::isAddress() const
{
	if (_is_address)
		return true;
	return false;
}
bool Server::isServerName() const
{
	if (_is_server_name)
		return true;
	return false;
}
void Server::pushBackLocation(Location & a_location)
{
	_v_location.push_back(a_location);
}

bool Server::checkListen(const std::string & alisten)
{
	if (_is_listen == true)
		return false;
	std::size_t found = alisten.find(":");
	if (found == std::string::npos)
		return false;
	_address = alisten.substr(0, found);
	_is_address = true;
	std::size_t port_length = alisten.size() - found;
	if (port_length != 5)
		return false;
	//if (isDigit(&alisten[found])
	for (unsigned int i = found + 1; i < alisten.size(); ++i)
	{
		if (alisten[i] < '0' || alisten[i] > '9')
			return false;
	}
	_port = std::atoi(alisten.substr(found + 1, alisten.size()).c_str());
	_is_listen = true;
	_is_port = true;
	return true;
}
bool Server::checkServerName(const std::string& aserver_name)
{
	if (_is_server_name == true)
		return false;
	_is_server_name = true;
	return true;
}

std::ostream& operator<<(std::ostream & ors, const Server & server)
{
	ors << "----SERVER----" << std::endl;
	ors << "listen" << server.getListen() << std::endl;
	ors << "address" << server.getAddress() << std::endl;
	ors << "port" << server.getPort() << std::endl;
	ors << "server_name" << server.getServerName() << std::endl;
	ors << "root" << server.getRoot() << std::endl;
	ors << "index" << server.getIndex() << std::endl;
	std::vector<std::string> v_methods = server.getMethods();
	for (unsigned int i = 0; i < v_methods.size(); i++) {
		ors << "methods[" << i << "]: " << v_methods[i] << std::endl;
	}
	std::map<int, std::string> m_errors = server.getErrorPage();
	std::map<int, std::string>::iterator it;
	for (it = m_errors.begin(); it != m_errors.end(); it++) {
		ors << "error_page: "
			<< it->first    // string (key)
			<< ':'
			<< it->second   // string's value 
			<< std::endl;
	}
	ors << "client_size" << server.getClientSize() << std::endl;
	ors << "autoindex" << server.getAutoindex() << std::endl;
	std::vector<Location> v_locations = server.getLocations();
	for (unsigned int i = 0; i < v_locations.size(); i++)
	{
		ors << "  ---Location [" << i << "]: " << v_locations[i] << std::endl;
	}
	return ors;
}
