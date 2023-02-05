#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include "Request.hpp"

class Response {

	private:

			std::map<std::string, std::string>		_m_headers;
			std::string								_response_content;
			int										_status_code;
			std::string								_response_body;

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
};




#endif