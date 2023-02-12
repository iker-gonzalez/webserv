#ifndef CLIENT_HPP
#define CLIENT_HPP


class Client 
{
	public:
		Client(int a_fd);
		Client();
		~Client();

		int		getClientFd() const;


	private:

		int		_client_fd;
};


#endif // !CLIENT_HPP