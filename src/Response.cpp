#include "../includes/Response.hpp"
#include "../includes/Utils.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <dirent.h>
#include <algorithm>


Response::Response()
{
	_response_content = "";
	_response_body = "";
	_status_code = 0;
	_target_file = "";
	_location = "";
	_auto_index = false;
	_isCGIResponse = 0;
}

Response::Response(Request &req) : request(req)
{
	_response_content = "";
	_response_body = "";
	_status_code = 0;
	_target_file = "";
	_location = "";
	_auto_index = false;
	_isCGIResponse = 0;

}

Response::~Response() {}

void	Response::setStatusLine()
{
//	_response_content.append("HTTP/1.1 " + std::to_string(_status_code) + " ");
	_response_content.append("HTTP/1.1 " + intToString(_status_code) + " ");
	_response_content.append("\r\n");
}

int		Response::readFile()
{
	std::ifstream file(_target_file.c_str());
	std::cerr << "\033[1;34mtarget file: " << _target_file << "\033[0m" << std::endl;
	std::ostringstream ss;
	if (file.fail())
	{
		_status_code = 404;
		std::string error_page = getErrorPage();
		if (!(error_page.empty()))
		{
			std::string root = "public/";
			_target_file = combinePaths(root, error_page, "");
			std::ifstream error_file(_target_file.c_str());
			ss << error_file.rdbuf();
			_response_body = ss.str();
		}
		return (1);
	}
	ss << file.rdbuf();
	_response_body = ss.str();
	return (0);
}

void	Response::setContentType()
{
	std::string file_extension = _target_file.substr(_target_file.find_last_of(".") + 1);
	std::string content_type = get_content_type(file_extension);
	//std::cout << "---CONTETNTYPE---" << content_type << std::endl;
	if (_auto_index)
		content_type = "text/html";
	_response_content.append("Content-Type: ");
	_response_content.append(content_type);
	_m_headers["Content-Type: "] = content_type;
	_response_content.append("\r\n");
}

void	Response::setContentLength()
{
	std::stringstream ss;
	ss << _response_body.length();
	_response_content.append("Content-Length: ");
	_response_content.append(ss.str());
	_response_content.append("\r\n");
	_m_headers["Content-Length: "] = ss.str();

}

void	Response::setConnection()
{
		if ((_m_headers["Connection: "].find("keep-alive")) != std::string::npos)
		{
			_response_content.append("Connection: keep-alive\r\n");
			_m_headers["Connection: "] = "keep-alive";
		}
}

void	Response::server()
{
	_response_content.append("Server: ");
	_response_content.append("nginx/1.14.0"); //? might need to change later
	_response_content.append("\r\n");
	_m_headers["Server: "] = "nginx/1.14.0";

}

void	Response::setDate()
{
	char date[1000];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	tm.tm_hour += 1;
	strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S %Z", &tm);
	_response_content.append("Date: ");
	_response_content.append(date);
	_response_content.append("\r\n");
	_m_headers["Date: "] = date;
}

void Response::setResponseBody(std::string &a_response_body)
{
	_response_body = a_response_body;
}
void Response::setResponseContent(std::string &ar_ResponseContent)
{
	_response_content = ar_ResponseContent;
}

int Response::isCGIResponse() const
{
	return _isCGIResponse;
}
void Response::setHeaders()
{
	setContentType();
	setContentLength();
	setConnection();
	server();
	location();
	setDate();
	_response_content.append("\r\n");
}
int		Response::createAutoIndexBody()
{
	DIR             *directoryPath;
    std::string     dirListPage;
	if (_target_file.empty())
		return 1;
	if (_target_file[_target_file.length() - 1] != '/')
	{
    	_target_file += "/";
		_status_code = 301;
	}
    directoryPath = opendir(_target_file.c_str());
	if (directoryPath == NULL)
	{
		std::cerr << "Could not open directory for autoindex creation" << std::endl;
		return 0;
	}
	_response_body == "<html>\n<head>\n<title>Index of" + _target_file + "</title>\n</head>\n<body>\n";
   _response_body += "<table style=\"width:80%; font-size: 15px\">\n";
   _response_body += "<hr>\n";
   _response_body += "<th style=\"text-align:left\"> File Name </th>\n";
   _response_body += "<th style=\"text-align:left\"> File Size </th>\n";

	struct dirent *directoryStruct;
	struct stat file_stat;
    std::string file_path;
    std::string file_name;
	int filesFound = 0;
	
	// Get all the directorys and list them
    while((directoryStruct = readdir(directoryPath)) != NULL)
    {
		file_name = directoryStruct->d_name;
		file_path = _target_file + directoryStruct->d_name;
       	if  (stat(file_path.c_str() , &file_stat) == 0)
		{
			if (directoryStruct->d_name[0] == '.')
				continue;
			filesFound++;
        	if (S_ISDIR(file_stat.st_mode))
				_response_body +=  "<tr>\n<td>\n<a href=\"" + file_name + "/\">" + file_name + "/</a>\n</td>\n</tr>\n";
			else
				_response_body +=  "<tr>\n<td>\n<a href=\"" + file_name + "\">" + file_name + "/</a>\n</td>\n</tr>\n";
		}
    }
	_response_body += "<p>Total files found : " + intToString(filesFound) + "</p>\n";
    _response_body.append("</body>\n");
    _response_body.append("</html>\n");
	if (closedir(directoryPath) == -1)
		return 1;
	return 0;
}

void	Response::buildResponse()
{
	// Create the body of the response
	int buildStatus = buildBody();
	if (_isCGIResponse)
	{
		return;
	}
	if (buildStatus)
	{
		
		std::cerr << _response_content << std::endl;
		if (!_isCGIResponse && !_auto_index)
    		std::cerr << "\x1B[33mError Building Body"  << std::endl;
	}

	// Set status line at the beginning of the Respose message
	setStatusLine();

	// Set Headers except for CGI case, in this case the script already set headers
	if (!_isCGIResponse)
		setHeaders();

	if (!(_response_body.empty()))
	{
		_response_content.append(_response_body);
	}

}

void	Response::findLocation(std::string path, std::vector<Location> locations, std::string &location_key)
{
	size_t biggest_match = 0;

	for(std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		if(path.find(it->getPath()) == 0)
		{
			if( it->getPath() == "/" || path.length() == it->getPath().length() || path[it->getPath().length()] == '/')
			{
				if(it->getPath().length() > biggest_match)
				{
					biggest_match = it->getPath().length();
					location_key = it->getPath();
				}
			}
		}
	}
}

bool Response::isMethodAllowed(std::string method, std::vector<std::string> allowed_methods)
{
	if ((allowed_methods.empty()))
	{
		if (_server.getMethods().empty())
		{
			if (method != "GET" && method != "POST")
				return false;
		}
	}
	for(std::vector<std::string>::const_iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it)
	{
		if (*it == method)
			return true;
	}
	return false;
}

bool	Response::checkIfReturn(Location &location)
{
	if (!location.getReturn().empty())
	{
		_location = location.getReturn();
		return true;
	}
	return false;
}

int Response::isClientSizeAllowed(int client_size_allowed, int location)
{
	if (location && !client_size_allowed)
	{
		if (_server.getClientSize())
			client_size_allowed = _server.getClientSize();
	}
	if (request.getContentLength() > client_size_allowed)
		return false;
	return true;
}

/*
	Check if the file specified by path is a directory or not. The stat function is used
	to obtain information about the file, and the information is stored in the file_stat structure.
	The S_ISDIR macro is then used to test if the file is a directory, and the result is returned.
*/

bool	Response::isDirectory(std::string path)
{
	struct stat file_stat;
	if (stat(path.c_str(), &file_stat) != 0)
		return (false);
	return (S_ISDIR(file_stat.st_mode));
}

/*
The return value of this function will be true if the file/directory exists
and is ready for reading, and false otherwise.
*/

bool Response::fileExists (const std::string& f)
{
	std::ifstream file(f.c_str());
	return (file.good());
}

std::string Response::combinePaths(std::string str1, std::string str2, std::string str3)
{
	int			len1;
	int			len2;

	len1 = str1.length();
	len2 = str2.length();
	if (str1[len1 - 1] == '/' && (!str2.empty() && str2[0] == '/') )
		str2.erase(0, 1);
	if (str1[len1 - 1] != '/' && (!str2.empty() && str2[0] != '/'))
		str1.insert(str1.end(), '/');
	if (str2[len2 - 1] == '/' && (!str3.empty() && str3[0] == '/') )
		str3.erase(0, 1);
	if (str2[len2 - 1] != '/' && (!str3.empty() && str3[0] != '/'))
		str2.insert(str1.end(), '/');
	return (str1 + str2 + str3);
}

std::string Response::getPath() const
{
	return _path;
}

CGI Response::getCGIResponse() const
{
	return _CGI_response;
}

void Response::setIsCGIResponse(int aiscgi)
{
	_isCGIResponse = aiscgi;
}
int Response::handleCGI(const Location &location, const std::string &a_Method)
{
	(void)a_Method;
	_isCGIResponse = 1;

	std::string content;
	content = request.getBody();

	std::string requestFile = request.getRequestFile();

	const std::string index_file = location.getIndex();

	_CGI_response.createCGIEnvironment(request, location);
	
	if (request.getMethod() == "DELETE")
	{
		size_t n_find = request.getBody().find("delete");
		if (n_find != std::string::npos)
		{
			std::string file_to_remove = "public/content/uploads/";
			file_to_remove += request.getBody().substr(7, file_to_remove.length() - 7);
			if ( !fileExists(file_to_remove))
			{
				_status_code = 404;
			}
			else
			{
				if (remove(file_to_remove.c_str()))
				{
					_status_code = 500;
				}
			}
		}
	}
		

	if (!_CGI_response.setupPipes(content))
		return 0;

	_CGI_response.execute(content);

	return 1;
}

int		Response::buildBody()
{
	// Before
	if (handleRequest())
	{
		std::string error_page = getErrorPage();
		if (!(error_page.empty()))
		{
			_target_file = combinePaths(_server.getRoot(), error_page, "");
			if (readFile())
				return (1);
			return (0);
		}
		return(1);
	}
	if (_status_code)
		return (0);
	if (_auto_index)
		return 0;
	if (request.getMethod() == "GET")
	{
		if (readFile())
			return (1);
	}
	else if (request.getMethod() == "POST")
	{
		if (fileExists(_target_file))
		{
			_status_code = 204;
			return (0);
		}
		std::ofstream file(_target_file.c_str(), std::ios::binary);
		if (file.fail())
		{
			_status_code = 403;
			return (1);
		}
		if (((request.getHeader("Content-Type:")).find("multipart/form-data")) != std::string::npos)
		{
				std::string content_type = request.getHeader("Content-Type:");
				size_t begin = content_type.find("boundary=") + 9;
				std::string boundary = content_type.substr(begin);
				// Use the boundary string to parse the request body
				std::string file_body = parseMultiPartRequest(request.getBody(), boundary);
				file.write(file_body.c_str(), file_body.length());
	
		}
		//code assumes that the request is a regular request, and it simply
		//writes the body of the request to the file
		else
			file.write(request.getBody().c_str(), request.getBody().length());
	}
	else if (request.getMethod() == "DELETE")
	{
		if (!fileExists(_target_file))
		{
			_status_code = 404;
			return (1);
		}
		std::string allow_directory = _target_file.substr(0, 23);
		if (allow_directory != "public/content/uploads/")
		{
			_status_code = 404;
			return (1);
		}
		if (remove(_target_file.c_str()))
		{
			_status_code = 500;
			return (1);
		}
	}
	_status_code = 200;

	return (0);
}


std::string Response::parseMultiPartRequest(const std::string& request_body, const std::string& boundary)
{
	std::vector<std::string> parts;
	size_t pos = 0;

	// split the request body into parts based on the boundary
	while ((pos = request_body.find(boundary, pos)) != std::string::npos)
	{
		size_t start_pos = pos + boundary.length();
		size_t end_pos = request_body.find(boundary, start_pos);
		parts.push_back(request_body.substr(start_pos, end_pos - start_pos));
		pos = end_pos;
	}

	// Find the part that contains the file data
	std::string result;
	for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it)
	{
		std::string part = *it;
		if (part.find("Content-Disposition: form-data; name=\"file\"") != std::string::npos)
		{
			size_t header_pos = part.find("\r\n\r\n");
			std::string data = part.substr(header_pos + 4); // Add 4 to remove the "\r\n\r\n" characters

			// Remove the boundary from the data
			size_t boundary_pos = data.rfind(boundary);
			if (boundary_pos != std::string::npos)
			{
				data.erase(boundary_pos);
			}

			// Append the data to the result string
			result += data;
			break;
		}
	}
	// Remove the last line (---) from the result string
	size_t last_newline_pos = result.find_last_of("\n");
	if (last_newline_pos != std::string::npos)
	{
		result.erase(last_newline_pos);
	}
	return result;
}

std::string	Response::getResponseContent()
{
	return(this->_response_content);
}

void    Response::setRequest(Request &req)
{
	request = req;
}

void     Response::setServer(Server &server)
{
	_server = server;
}

// This function checks if the requested file is a directory and appends a slash if needed.
// If a slash is added, it returns 1 and sets the path variable to the new request file.
int Response::handleDirectory(Location& target_location)
{
	if (_target_file[_target_file.length() - 1] != '/')
	{
		_location = request.getRequestFile() + "/";
		_status_code = 301;
		return (1);
	}
	if (!target_location.getIndex().empty())
	{
		_target_file += target_location.getIndex();
		return 0;
	}
	// Check if autoindex is on
	else if (target_location.getAutoindex())
	{
		// allows to generate a directory listing for a given location
			_auto_index = true;
		if (_target_file[_target_file.length() - 1] != '/')
		{
    		_target_file += "/";
			_status_code = 301;

		}
		int error = createAutoIndexBody();
		if (error)
			_status_code = 500;
		else if (!error && _status_code != 301) //We previusly modify request file
			_status_code = 200;
		return 0;
	}
	// Add index of the server
		else
		_target_file += _server.getIndex();	

	// If the created _target_file does not exist
	if (!fileExists(_target_file))
	{
		if (isDirectory(_target_file))
		{
			_status_code = 301;
			if (!target_location.getIndex().empty())
				_location = combinePaths(request.getRequestFile(), target_location.getIndex(), "");
			else
				_location = combinePaths(request.getRequestFile(), _server.getIndex(), "");
			if (_location[_location.length() - 1] != '/')
				_location.insert(_location.end(), '/');
			return (1);
		}
	}
	return 0;
}

// This function handles the case where the requested file is not matched by a location.
// It sets the target file to the path combined with the server root and calls handleDirectory.
int Response::handleNoMatch() 
{
	_target_file = combinePaths(_server.getRoot(), request.getRequestFile(), "");
	
	if(!(isMethodAllowed(request.getMethod(),_server.getMethods())))
	{
		_status_code = 403; 
		return (1);
	}
	if (!(isClientSizeAllowed(_server.getClientSize(), 0)))
	{
		_status_code = 413;
		return (1);
	}

	if (isDirectory(_target_file))
	{
		if (_target_file[_target_file.length() - 1] != '/')
		{
			_status_code = 301;
			_location = request.getRequestFile() + "/";
			return 1;
		}
		_target_file += _server.getIndex();
		// if there is not index on this directory...
		if (!fileExists(_target_file))
		{
			std::cerr << "\033[31mError: Directory requested and not index for it: \033[0m" << _target_file << std::endl;
			_status_code = 404;
			return 1;
		}
		// if the index specified for this directory is another directory...
		if (isDirectory(_target_file))
		{
			_status_code = 301;
			_location = combinePaths(request.getRequestFile(), _server.getIndex(), "");
			if(_location[_location.length() - 1] != '/')
				_location.insert(_location.end(), '/');
			return 1;
		}
	}
	return (0);
}

Location	Response::findLocationByName(std::string target_location, std::vector<Location> locations)
{
	for(std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		if ((strcmp(target_location.c_str(), it->getPath().c_str())) == 0)
			return *it;
	}
	return Location();
}

// This function handles the case where the requested file is matched by a location.
// It sets the target file to the path combined with the location root or alias, depending on which is specified.
// It then calls handleDirectory.
int Response::handleMatch(std::string location)
{
	Location target_location = findLocationByName(location, _server.getLocations());

	// Get target file with the relative path
	if (!(target_location.getAlias().empty()))
		_target_file = combinePaths(target_location.getAlias(), request.getRequestFile().substr(target_location.getPath().length()), "");
	else
	{
		if(!target_location.getRoot().empty())
			_target_file = combinePaths(target_location.getRoot(), request.getRequestFile(), "");
		else
			_target_file = combinePaths(_server.getRoot(), request.getRequestFile(), "");
	}
	if (!(isMethodAllowed(request.getMethod(), target_location.getMethods()))) {
		_status_code = 405;
		return 1;
	}
	if (!(isClientSizeAllowed(target_location.getClientSize(), 1)))
	{
		_status_code = 413;
		return 1;
	}

	if (checkIfReturn(target_location))
	{
		// If there is a return 
		_status_code = 301;
		return 1;
	}
	if (target_location.getPath().find("cgi-bin") != std::string::npos)
	{
		return handleCGI(target_location, request.getMethod());
	}
	if (isDirectory(_target_file) && request.getMethod() != "POST")
	{
		return (handleDirectory(target_location));
	}
	return (0);
}

// This is the main function that handles a request.
// It first tries to find a matching location and calls handleMatch or handleNoMatch depending on the result.
// It returns 0 on success, 1 on failure.
int Response::handleRequest() 
{
	std::string location_match;
	
	findLocation(request.getRequestFile(), _server.getLocations(), location_match);
	
	if (location_match.empty())
		return handleNoMatch();
	else
		return handleMatch(location_match);
}

int Response::getStatusCode()
{
	return _status_code;
}

void 	Response::location()
{
	if (_location.length())
	{
		_response_content.append("Location: "+ _location +"\r\n");
		_m_headers["Location: "] = _location;
	}
}

std::string		Response::get_content_type(std::string file_extension)
{
	std::cerr << "file extension:" << file_extension << std::endl;
	if (file_extension == "html" || file_extension == "htm")
		return "text/html";
	else if (file_extension == "json")
		return "application/json";
	else if (file_extension == "css")
		return "text/css";
	else if (file_extension == "xml")
		return "application/xml";
	else if (file_extension == "png")
		return "image/png";
	else if (file_extension == "jpeg" || file_extension == "jpg")
		return "image/jpeg";
	else if (file_extension == "gif")
		return "image/gif";
	else if (file_extension == "pdf")
		return "text/plain";
		//return "application/pdf";
	else if (file_extension == "txt")
		return "text/plain";
	else
		return "application/octet-stream";
}

std::string	Response::getErrorPage(void)
{
	std::map<int, std::string> errorPages = _server.getErrorPage();
	std::map<int, std::string>::const_iterator it;
	for (it = errorPages.begin(); it != errorPages.end(); ++it)
	{
		if (it->first == _status_code)
			return (errorPages[_status_code]);
	}
	return "";
}

