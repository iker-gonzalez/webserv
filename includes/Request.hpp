#include <map>
#include <iostream>

class Request
{
	private:
		int client_fd;
	public:
		std::string method;
		std::string path;
		std::map<std::string, std::string> headers;
		//std::string body;
		size_t	content_length;
		Request (int client_fd);
		~Request();

		Request parsing(std::string request);
};