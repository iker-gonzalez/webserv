#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <iostream>

class Request
{
	private:
		int									_client_fd;
		std::string							_method; 	// GET, POST or DELETe
		std::string							_requestFile; 	// Request HTML file
		std::map<std::string, std::string>	_m_headers;	// All information
		size_t								_content_length;
	public:

		Request ();
		~Request();

		//Parseo
		int requestParsing( std::string& request);
		
		//Getters
		int									getClientFd(void) const;
		std::string							getMethod(void) const;
		std::string							getRequestFile(void) const;
		std::map<std::string, std::string>	getHeaders(void) const;
		size_t								getContentLength(void) const;

};

#endif
