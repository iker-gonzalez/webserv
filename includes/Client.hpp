#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Request.hpp"
#include "Response.hpp"


class Client 
{
	public:
		Client(int a_fd);
		Client();
		~Client();

		int		getClientFd() const;
		void	buildResponse();


		Request		request;
		Response	response;

	private:

		int		_client_fd;
};


#endif // !CLIENT_HPP