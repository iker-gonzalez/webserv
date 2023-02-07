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
	Server(const Server &other);
    Server &operator=(const Server & rhs);
	
	//***********Parseo**********
	bool fillSpecificInfo(std::vector<std::string>& a_v_strSplit);
	void pushBackLocation(Location& a_location);
	bool checkListen(const std::string& alisten);

	//***********Setups sever**********
	// Creating a socket using socket, set as not blocking (fcntl) and bind.
	bool setupServer(void);
	bool createSocket(void);
	bool nonBlokingSocket(void);
	bool bindSocket(void);
    bool listenConnections(); 
	
	//Getters
	int 					getPort() const;
	std::string 			getAddress() const;
	std::vector<Location> 	getLocation() const;
	int					 	getListenFd() const;
	Location& 				getLocationByReference(int nbr_localitaion) ;

	//Tengo que poner esto en privado
	bool locationBracketOpen;
private:
	int 					_port;
	std::string 			_address;
	std::vector<Location> 	_v_location;
	int						_listen_fd;
};

std::ostream& operator<<(std::ostream& ors, const Server& server);

#endif // !SERVER_HPP

