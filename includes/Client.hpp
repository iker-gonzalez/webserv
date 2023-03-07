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

		int		getClientFd() const;
		void	buildResponse();
		void    setServer(Server &serv);



		Request		request;
		Response	response;
		Server		server;

	private:

		int		_client_fd;
};


#endif // !CLIENT_HPP