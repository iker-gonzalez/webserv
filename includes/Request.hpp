#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <iostream>
#include <sys/socket.h>

class Request
{
	private:
		int									_client_fd;
		std::string							_request_body;
		std::string							_method; 	// GET, POST or DELETe
		std::string							_requestFile; 	// Request HTML file
		std::map<std::string, std::string>	_m_headers;		// All information
		size_t								_content_length;
		std::string							_serverName;
		int									_port;
		

	public:

		Request ();
		~Request();

		//Parseo
		int parseHeaders(std::string& request);
		int	parseBody();
		
		//Getters
		int									getClientFd(void) const;
		std::string							getMethod(void) const;
		std::string							getRequestFile(void) const;
		std::string							getBody(void) const;
		std::map<std::string, std::string>	getHeaders(void) const;
		size_t								getContentLength(void) const;
		std::string							parseChunkedBody();
		std::string							getServerName() const;
		int									getPort() const;
};

#endif
