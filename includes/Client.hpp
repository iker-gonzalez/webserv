#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include <time.h>

class Client 
{
	public:
		Client(Server &serv, int a_fd);
		Client();
		~Client();

		// Getters
		int		getClientFd() const;
		Server	getServer() const;
		time_t	getTimeOut() const;
		void	buildResponse();
		void    setServer(Server &serv);

		// Objects
		Request		request;
		Response	response;
		Server		server;

		// Check
		int	getIsCGI() const;

		// Setter
		void	setIsCGI(int a_is_CGI) ;

	private:
		int		_client_fd; 
		time_t  _time_out;
};


#endif // !CLIENT_HPP