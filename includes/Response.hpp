#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <ctime>
#include "sys/stat.h"
#include "Request.hpp"
#include "Server.hpp"

class Response {

	private:

			Server									_server;
			std::map<std::string, std::string>		_m_headers;
			std::string								_response_content;
			int										_status_code;
			std::string								_response_body;
			std::string								_path;
			std::string								_target_file;
			bool									_auto_index;

	public:

			Response();
			Response::Response(Request &request);
			~Response();

			Request		request;
			void		buildResponse();
			int			buildBody();
			int			readFile();
			void		setStatusLine();
			void		setHeaders();
			void		setContentType();
			void		setContentLength();
			void		setConnection();
			void		setServer();
			void		setDate();
			int			handleRequest();
			std::string	findLocation(std::string request_file, std::vector<Location> locations, int &index);
			bool		isMethodAllowed(std::string method, Location &location);
			bool		checkIfReturn(Location &location);
			bool		isDirectory(std::string path);
			bool		fileExists (const std::string& f);
			int			isClientSizeAllowed(Location &location);
};




#endif