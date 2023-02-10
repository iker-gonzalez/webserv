#ifndef SERVER_HPP
#define SERVER_HPP

#include "CommonInfo.hpp"
#include "Location.hpp"
#include <vector>
#include <ostream>

class Server : public CommonInfo
{
public:
	Server();
	~Server();
	Server(const Server& other);
	Server& operator=(const Server& rhs);

	//***********Parseo**********
	bool fillSpecificInfo(std::vector<std::string>& a_v_strSplit);

	void pushBackLocation(Location& a_location);

	//Check
	bool checkServerName(const std::string& aserver_name);
	bool checkListen(const std::string& alisten);

	//***********Setups sever**********
	// Creating a socket using socket, set as not blocking (fcntl) and bind.
	bool setupServer(void);
	bool createSocket(void);
	bool nonBlokingSocket(void);
	bool bindSocket(void);
	bool listenConnections();

	// Getters
	int 					getPort() const;
	std::string 			getAddress() const;
	std::vector<Location> 	getLocations() const;
	std::string				getServerName()const;
	int					 	getListenFd() const;
	Location& 				getLocationsByReference(int nbr_localitaion);

	// Set
	void setServerName(const std::string& aserver_name);

	// Is
	bool isPort() const;
	bool isServerName() const;
	bool isAddress() const;

	//Tengo que poner esto en privado
	bool locationBracketOpen;
private:
	int 					_port;
	std::string 			_address;
	std::vector<Location> 	_v_location;
	std::string				_server_name;
	int						_listen_fd;

	// Check attributes
	bool _is_port;
	bool _is_address;
	bool _is_server_name;
};

std::ostream& operator<<(std::ostream& ors, const Server& server);

#endif // !SERVER_HPP

