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

		std::string							_request_header;
		std::string							_request_body;
		std::string							_request_raw;   // _header_raw + request_body 

		// Information from header request
		std::string							_method; 		// GET, POST or DELETe
		std::string							_requestFile; 	// Request HTML file (ej. /cgi-bin)
		std::map<std::string, std::string>	_m_headers;		// All information
		size_t								_content_length; // Donde se recoge??
		std::string							_serverName;
		int									_port;
		bool								_is_chunked;
		

	public:

		Request ();
		~Request();
    	Request &operator=(const Request & rhs);

		// 1. Read request
		bool 								parseRequest(int client_fd);
		
		// 1.2. Headers Parseo
		bool 								readHeaderRequest(int client_fd);
		bool								parseHeaderRequest(void);
		void								parseHeaderHelper();
		
		// 1.2. Body Parseo
		bool								parseBodyRequest(int client_fd);
		bool								parseNotChunkedBody(int client_fd);
		bool								parseChunkedBody(int client_fd) ;

		//Parseo
		bool								parseHeaders(std::string& request);
		
		// Useful for clean code

		//Getters
		int									getClientFd(void) const;
		std::string							getMethod(void) const;
		std::string							getRequestFile(void) const;
		std::string							getBody(void) const;
		std::map<std::string, std::string>	getHeaders(void) const;
		size_t								getContentLength(void) const;
		std::string							getServerName() const;
		int									getPort() const;
		std::string 						getHeader(std::string const &name) const;
		std::string 						getRawBody() const;
		bool								getIsChunked(void) const;
		std::string							getResquestHeaderStr(void) const;

		void								setBody(std::string &a_body);
};

std::ostream& operator<<(std::ostream& ors, const Request& a_request);

#endif
