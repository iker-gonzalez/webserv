#include "../includes/Response.hpp"
#include "../includes/Utils.hpp"
#include <sstream>
#include <string>
#include <iostream>


Response::Response()
{
	_response_content = "";
	_response_body = "";
	_status_code = 0;
	_target_file = "";
	_location = "";
	_auto_index = false;
	_isCGIResponse = false;
}

Response::Response(Request &req) : request(req)
{
	_response_content = "";
	_response_body = "";
	_status_code = 0;
	_target_file = "";
	_location = "";
	_auto_index = false;
	_isCGIResponse = false;

}

Response::~Response() {}

void	Response::setStatusLine()
{
	_response_content.append("HTTP/1.1 " + std::to_string(_status_code) + " ");
	_response_content.append("\r\n");
}

int		Response::readFile()
{
	std::ifstream file(_target_file.c_str());
	std::cout << "\033[1;34mtarget file: " << _target_file << "\033[0m" << std::endl;
	if (file.fail())
	{
		_status_code = 404;
		return (1);
	}
	std::ostringstream ss;
	ss << file.rdbuf();
	_response_body = ss.str();
	return (0);
}

void	Response::setContentType()
{
	std::string file_extension = _target_file.substr(_target_file.find_last_of(".") + 1);
	std::string content_type = get_content_type(file_extension);
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
		if (((request.getHeader("Connection:")).find("keep-alive")) != std::string::npos)
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
		{ 
	char date[1000];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S %Z", &tm);
	_response_content.append("Date: ");
	_response_content.append(date);
	_response_content.append("\r\n");
	_m_headers["Date: "] = date;
	return;


	}
	{
	/* 
	get current time
	nullptr is used as an argument to indicate that the function should use 
	the local time instead of the time from the system clock.
	*/
	std::time_t now = std::time(nullptr);

	/* 
	convert the now time to a std::tm structure, which represents the broken-down 
	time in the Coordinated Universal Time (UTC) format. The * operator is used to
	dereference the pointer returned by std::gmtime and store the result in the tm variable.
	*/
	std::tm tm = *std::gmtime(&now);
	//set time to UTC + 1
	tm.tm_hour += 1;

	std::ostringstream date;
	//used to format the tm structure into a string
	//date << std::put_time(&tm, "%a, %d %b %Y %T %Z");

	//_response_content.append("Date: ");
	//_response_content.append(date.str());
	//_response_content.append("\r\n");
	//_m_headers["Date: "] = date.str();


	}
}

bool Response::isCGIResponse() const
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

void	Response::buildResponse()
{
	int error;
	error = buildBody();
	
		setStatusLine();
	if (!_isCGIResponse)
		setHeaders();
	//if (error && !_isCGIResponse) //??Siempre que sale CGI entra en error asique he tenido que poner esta condicion
	//	ErrorPage();
	
	if (request.getMethod() == "GET")
	{
		_response_content.append(_response_body);
	}
	//std::cerr << _response_content << std::endl;
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
		//if (_location[0] != '/')
		//	_location.insert(0, 1, '/');
		return true;
	}
	return false;
}

int Response::isClientSizeAllowed(Location &location)
{
	std::cout << "check 1\n";
	if (!(location.getClientSize()))
	{
		std::cout << "check 2\n";

		if (_server.getClientSize())
		{
					std::cout << "check 3\n";
			std::cout << "request content lenght: " << request.getContentLength() << std::endl;
			std::cout << "server client size: " << _server.getClientSize() << std::endl;
			if (request.getContentLength() < _server.getClientSize())
				return true;
		}
					std::cout << "check 4\n";

		if (request.getContentLength() < DEFAULT_CLIENT_MAX_BODY_SIZE)
			return true;
					std::cout << "check 5\n";

	}
	else
	{
		if(request.getContentLength() < location.getClientSize())
			return true;
	}
	return false;
}

void Response::ErrorPage()
{
	_response_content.append(buildErrorPage(_status_code));
	_target_file = "public/content/html/error.html";
}

/*
	Check if the file specified by path is a directory or not. The stat function is used
	to obtain information about the file, and the information is stored in the file_stat structure.
	The S_ISDIR macro is then used to test if the file is a directory, and the result is returned.
*/

bool	isDirectory(std::string path)
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

bool fileExists (const std::string& f)
{
	std::ifstream file(f.c_str());
	return (file.good());
}

std::string combinePaths(std::string str1, std::string str2, std::string str3)
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
int Response::handleCGI(const Location &location)
{
	_isCGIResponse= true;
	//std::cout << "CGI:Request File" << request.getRequestFile() << std::endl;
	std::string requestFile = request.getRequestFile();

	const std::string index_file = location.getIndex();

	_CGI_response.createCGIEnvironment(request, location);
	_CGI_response.setupPipes();
	//_response_body.clear();
	_response_body = _CGI_response.execute();


	//exit(0);
	return true;
}

int		Response::buildBody()
{
	if (handleRequest())
		return (1);
	if (_status_code)
		return (0);
	if (request.getMethod() == "GET")
	{
		if (readFile())
			return (1);
	}
	else if (request.getMethod() == "POST")
	{
		std::cout << "TARGEEEEEET:\n" << _target_file << std::endl;
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
				std::cout << "file body:\n";
				std::cout << file_body << std::endl;
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
		/*
		If the file exists and can be successfully deleted, the function returns 0.

		If the file does not exist or if there was an error deleting the file, the 
		function returns a non-zero value to indicate an error.
		*/
		if (remove(_target_file.c_str()))
		{
			_status_code = 500;
			return (1);
		}
	}
	_status_code = 200;

	// CGI Case
	return (0);
}


std::string Response::parseMultiPartRequest(const std::string& request_body, const std::string& boundary)
{
	std::vector<std::string> parts;
	size_t pos = 0;

	std::cout << "PARSEAMOS MULTIII VAMOSSS" << std::endl;
	std::cout << request_body << std::endl;
	std::cout << "boundary: "<< boundary << std::endl;
	// split the request body into parts based on the boundary
	while ((pos = request_body.find(boundary, pos)) != std::string::npos)
	{
		size_t start_pos = pos + boundary.length();
		size_t end_pos = request_body.find(boundary, start_pos);
		parts.push_back(request_body.substr(start_pos, end_pos - start_pos));
		pos = end_pos;
	}

	// Concatenate all the data from the parts into a single string
	std::string result;
	for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it)
	{
		std::string part = *it;
		size_t header_pos = part.find("\r\n\r\n");
		std::string data = part.substr(header_pos + 4); // Add 4 to remove the "\r\n\r\n" characters

		// Append the data to the result string
		result += data;
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
int Response::handleDirectory(Location target_location)
{
	if (_target_file[_target_file.length() - 1] != '/')
	{
		_location = request.getRequestFile() + "/";
		_status_code = 301;
		return (1);
	}
	if (!target_location.getIndex().empty())
		_target_file += target_location.getIndex();
	else
		_target_file += _server.getIndex();
	std::cout << "target file 2:" << _target_file << std::endl;
	if (!fileExists(_target_file))
	{
		// allows to generate a directory listing for a given location
		if (target_location.getAutoindex())
		{
			_target_file.erase(_target_file.find_last_of('/') + 1);
			_auto_index = true;
			return (0);
		}
		else
				{
			_status_code = 404;
					return (1);
		}
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
	std::cout << std::endl;
	std::cout << "\033[33m" << "SERVER" << "\033[0m" << std::endl;
	_target_file = combinePaths(_server.getRoot(), request.getRequestFile(), "");
	if (isDirectory(_target_file))
	{
		std::cout << "is directoryyy\n";
		std::cout << "target file:" << _target_file << std::endl;
		if (_target_file[_target_file.length() - 1] != '/')
		{
			std::cout << "redirect\n";
			_status_code = 301;
			_location = request.getRequestFile() + "/";
			std::cout << "location:" << _location << std::endl;
			return 1;
		}
		_target_file += _server.getIndex();
		// if there is not index on this directory...
		if (!fileExists(_target_file))
		{
			std::cout << "\033[31mError: Directory requested and not index for it: \033[0m" << _target_file << std::endl;
			_status_code = 404;
			return 1;
		}
		// if the index specified for this directory is another directory...
		if (isDirectory(_target_file))
		{
			std::cout << "\033[31mRedirection: Directory requested and index is another directory: \033[0m" << _target_file << std::endl;
			_status_code = 301;
			_location = combinePaths(request.getRequestFile(), _server.getIndex(), "");
			if(_location[_location.length() - 1] != '/')
				_location.insert(_location.end(), '/');
			std::cout << "\033[31mRedirected to: \033[0m" << _location << std::endl;
			
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
	std::cout << std::endl;
	std::cout << "\033[38;5;205mLOCATION\033[0m" << std::endl;
	Location target_location = findLocationByName(location, _server.getLocations());
	if (!(isMethodAllowed(request.getMethod(), target_location.getMethods()))) {
		_status_code = 405;
		return 1;
	}
	if (!(isClientSizeAllowed(target_location))) {
		_status_code = 413;
		return 1;
	}
	if (checkIfReturn(target_location)) {
		std::cout << "RETURN\n";
		_status_code = 301;
		return 1;
	}

		if (target_location.getPath().find("cgi-bin") != std::string::npos)
		{
			return handleCGI(target_location);
		}
	//! HANDLE CGI

		if (!(target_location.getAlias().empty()))
			_target_file = combinePaths(target_location.getAlias(), request.getRequestFile().substr(target_location.getPath().length()), "");
		else
		{
			if(!target_location.getRoot().empty())
				_target_file = combinePaths(target_location.getRoot(), request.getRequestFile(), "");
			else
				_target_file = combinePaths(_server.getRoot(), request.getRequestFile(), "");
		}
		std::cout << "target file: " << _target_file << std::endl;
		

	//! HANDLE CGI

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
	int index;
	index = 0;
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
	std::cout << "location: " << _location << std::endl;
	if (_location.length())
	{
		_response_content.append("Location: "+ _location +"\r\n");
		_m_headers["Location: "] = _location;
	}
}

std::string		Response::get_content_type(std::string file_extension)
{
	if (file_extension == "html" || file_extension == "htm")
		return "text/html";
	else if (file_extension == "json")
		return "application/json";
	else if (file_extension == "xml")
		return "application/xml";
	else if (file_extension == "png")
		return "image/png";
	else if (file_extension == "jpeg" || file_extension == "jpg")
		return "image/jpeg";
	else if (file_extension == "gif")
		return "image/gif";
	else if (file_extension == "pdf")
		return "application/pdf";
	else if (file_extension == "txt")
		return "text/plain";
	else
		return "application/octet-stream";
}

