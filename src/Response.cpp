#include "../includes/Response.hpp"
#include "Response.hpp"

Response::Response()
{
	_response_content = "";
	_response_body = "";
	_status_code = 0;
	_target_file = "";
	_path = "";
	_auto_index = false;
	_server = "";
}

Response::Response(Request &req) : request(req)
{
	_response_content = "";
	_response_body = "";
	_status_code = 0;
	_target_file = "";
	_path = "";
	_auto_index = false;
	_server = "";
}

Response::~Response() {}

void	Response::setStatusLine()
{
	_response_content.append("HTTP/1.1 " + std::to_string(_status_code) + " ");
	_response_content.append("\r\n");
}

int		Response::readFile()
{
	std::ifstream file(request.getRequestFile().c_str());

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
	_response_content.append("Content-Type: ");
	_response_content.append("text/html"); //? should any other content type different from html be considered?
	_m_headers["Content-Type: "] = "text/html";

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
	if(request.getHeaders()["Connection: "] == "keep-alive")
	{
		_response_content.append("Connection: keep-alive\r\n");
		_m_headers["Connection: "] = "keep-alive";
	}

}

void	Response::setServer()
{
	_response_content.append("Server: ");
	_response_content.append("nginx/1.14.0"); //? might need to change later
	_response_content.append("\r\n");
	_m_headers["Server: "] = "nginx/1.14.0";

}

void	Response::setDate()
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

	std::ostringstream date;
	//used to format the tm structure into a string
	date << std::put_time(&tm, "%a, %d %b %Y %T %Z");

	_response_content.append("Date: ");
	_response_content.append(date.str());
	_response_content.append("\r\n");
	_m_headers["Date: "] = date.str();
}

void	Response::setHeaders()
{
	setContentType();
	setContentLength();
	setConnection();
	setServer();
	setDate();
	//?location
	_response_content.append("\r\n");
}

void	Response::buildResponse()
{
	if (buildBody())
		buildErrorBody();
	setStatusLine();
	setHeaders();
	if (request.getMethod() == "GET")
		_response_content.append(_response_body);
}

std::string	findLocation(std::string request_file, std::vector<Location> locations, int &index)
{
	for(std::vector<Location>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		if (strcmp(request_file.c_str(), it->getPath().c_str()))
			return request_file;
		index++;
	}

	return "";
}

bool Response::isMethodAllowed(std::string method, Location &location)
{
	std::vector<std::string> allowed_methods = location.getMethods();
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
		_path = location.getReturn();
		if (_path[0] != '/')
			_path.insert(0, 1, '/');
		return true;
	}
	return false;
}

int Response::isClientSizeAllowed(Location &location)
{
	if (!(location.getClientSize()))
	{
		if (_server.getClientSize())
		{
			if (request.getContentLength() < _server.getClientSize())
				return true;
		}
		if (request.getContentLength() < 1MB)
			return true;
	}
	if (request.getContentLength() < location.getClientSize())
		return true;
	return false;
}

void Response::buildErrorBody()
{
	
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
The return value of this function will be true if the file/directory exists and is ready for reading, and false otherwise.
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

int		Response::handleRequest()
{
	std::string	location_match;
	int			index;

	index = 0;
	location_match = findLocation(request.getRequestFile(), _server.getLocations(), index);
	if (location_match.length() > 0)
	{
		Location	target_location = _server.getLocationByReference(index);
		if (!(isMethodAllowed(request.getMethod(), target_location)))
		{
			_status_code = 405;
			return (1);
		}
		if (!(isClientSizeAllowed(target_location))) //?must check if metrics retrieved are the right ones
		{
			_status_code = 413;
			return (1);
		}
		if (checkIfReturn(target_location))
		{
			_status_code = 301;
			return (1);
		}
		
		//! HANDLE CGI WHEN WE HAVE THE CLASS CREATED

		if (!(target_location.getAlias().empty()))
			_target_file = combinePaths(target_location.getAlias(), request.getRequestFile().substr(target_location.getPath().length()), "");
		else
			_target_file = combinePaths(target_location.getRoot(), request.getRequestFile(), "");
		
		//! HANDLE CGI WHEN WE HAVE THE CLASS CREATED
		
		if (isDirectory(target_location.getPath()))
		{
			if (_target_file[_target_file.length() - 1] != '/')
			{
				_path = request.getRequestFile() + "/";
				_status_code = 301;
				return (1);
			}
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
			}
		}
	}
	else
	{
		_target_file = combinePaths(_server.getRoot(), request.getRequestFile(), "");
		if (isDirectory(_target_file))
		{
			_status_code = 301;
			_path = request.getRequestFile() + "/";
		}
		// the index directive specifies the default file name that should be served when a directory is requested
		_target_file += _server.getIndex();
		if (!fileExists(_target_file))
		{
			_status_code = 404;
			return (1);
		}
		//? still determining this logic
	}
	return (0);
}

int		Response::buildBody()
{
	if (handleRequest())
		return (1);
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
			_status__code = 404;
			return (1);
		}
		/*
			A multi-part form request is a type of HTTP request used to submit large
			amounts of data, such as binary files, to a server.

			In a multi-part form request, the request body is divided into several parts,
			each of which can contain different types of data, such as text fields, binary files.

			Each part is separated by a boundary string, which is specified in the "Content-Type" header of the HTTP request.

			When a server receives a multi-part form request, it can parse the request body into its individual parts, process
			each part separately, and store the data in a suitable format, such as a file or a database.
		*/
		if (//! handle if request multi-part form request)
		{

		}
		/*
		 	code assumes that the request is a regular request, and it simply
			writes the body of the request to the file
		*/
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
	return (0);
}