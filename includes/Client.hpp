#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"


class Client 
{
	public:
		Client(Server &serv, int a_fd);
		Client();
		~Client();

		// Getters
		int		getClientFd() const;
		void	buildResponse();
		void    setServer(Server &serv);

		// Objects
		Request		request;
		Response	response;
		Server		server;

		// Check
		bool	getIsCGI() const;

		// Setter
		void	setIsCGI(bool a_is_CGI) ;

		// Poner private
		//Request _request;

	private:
		int		_client_fd; 
		bool	_is_CGI;
};


#endif // !CLIENT_HPP