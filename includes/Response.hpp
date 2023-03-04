#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <ctime>
#include <cstdio>
#include "sys/stat.h"
#include "Request.hpp"
#include "Server.hpp"

#define DEFAULT_CLIENT_MAX_BODY_SIZE 1000000

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
	Response(Request& request);
	Response(Server& ar_server);
	~Response();

	Request		request;
	void		buildResponse();
	int			buildBody();
	int			readFile();
	void		setRequest(Request &req);
	void		setStatusLine();
	void		setHeaders();
	void		setContentType();
	void		setContentLength();
	void		setConnection();
	void		setServer();
	void		setDate();
	int			handleRequest();
	std::string	findLocation(std::string request_file, std::vector<Location> locations, int& index);
	bool		isMethodAllowed(std::string method, std::vector<std::string> allowed_methods);
	bool		checkIfReturn(Location& location);
	//bool		isDirectory(std::string path);
	//bool		fileExists(const std::string& f);
	int			isClientSizeAllowed(Location& location);
	void		parseMultiPartRequest(const std::string& request_body, const std::string& boundary);
	//int			ft_check_errors(std::string target_file, Location target_location);
	void		ErrorPage();
	std::string	getResponseContent();
};


#endif