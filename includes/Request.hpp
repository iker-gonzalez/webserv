#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <iostream>
#include <sys/socket.h>

class Request
{
	private:
		// _client_fd: Esto no es nesario si el cliente va a tener la Request guardada no??
		int									_client_fd;

		std::string							_request_body;
		std::string							_method; 		// GET, POST or DELETe
		std::string							_requestFile; 	// Request HTML file (ej. /cgi-bin)
		std::map<std::string, std::string>	_m_headers;		// All information
		size_t								_content_length; // Donde se recoge??
		std::string							_serverName;
		int									_port;
		

	public:

		Request ();
		~Request();

		//Parseo
		bool parseHeaders(std::string& request);
		int	parseBody(int client_fd);
		std::string							parseChunkedBody(int client_fd) const;
		bool 								parseRequest(std::string request, int client_fd);
		
		//Getters
		int									getClientFd(void) const;
		std::string							getMethod(void) const;
		std::string							getRequestFile(void) const;
		std::string							getBody(void) const;
		std::map<std::string, std::string>	getHeaders(void) const;
		size_t								getContentLength(void) const;
		std::string							getServerName() const;
		int									getPort() const;
		std::string 						getHeader(std::string const &name);

};
std::ostream& operator<<(std::ostream& ors, const Request& a_request);

#endif
