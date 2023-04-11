#include "../includes/Request.hpp"
#include "../includes/Utils.hpp"
#include <string>
#include <cstdlib>

Request::Request() : _client_fd(0),  _content_length(0), _port(0)
		 	
{
	_request_header = "";
	_request_body = "";
	_request_raw = "";

	_is_chunked = false;

	_content_length = 0;
	_client_fd = 0;
	_method = "";
	_requestFile = "";
	_content_length = 0;
}
Request &Request::operator=(const Request &rhs)
{
    _client_fd = rhs._client_fd;

	_request_header = rhs._request_header;
	_request_body = rhs._request_body;
	_request_raw = rhs._request_raw;

	_method	= rhs._method;
	_requestFile = rhs._requestFile;
	_m_headers = rhs._m_headers;
	_content_length = rhs._content_length;
	_serverName = rhs._serverName;
	_port = rhs._port;
	_is_chunked = rhs._is_chunked;

	return *this;
}

Request::~Request()
{
	
}

//****************PARSEO*********************
bool Request::parseRequest(int client_fd)
{
	std::cerr << "READ_HRADERS\n";
	if (!readHeaderRequest(client_fd))
		return false;
	
	std::cerr << "PARSE_HEADERS\n";
	//Si los bytes son 0 cerrar fd
	if (_request_header.empty())
		return true; //?? No me gusta como gestionamos cuando entra una petición vacia. Mirar
	
	if (!parseHeaderRequest())
		return false;
	
	std::cerr << "PARSE_BODY\n";
	if (!parseBodyRequest(client_fd))
		return false;
	std::cerr << "FINISH_PARSE_BODY\n";
	_request_raw = _request_header + _request_body;
	return true;
}

bool Request::readHeaderRequest(int client_fd)
{
	ssize_t bytes_read;
	size_t end;
	char c;
	int total_bytes_read;

	total_bytes_read = 0;
	std::cerr << "readHeaderRequest" << client_fd << std::endl;
	while ((end = _request_header.find("\r\n\r\n")) == std::string::npos)
	{
		if ((bytes_read = recv(client_fd, &c, 1, 0)) == -1 )
		{
				std::cerr << bytes_read <<  "Error receivingdw\n";
				return false;
		}
		else if (!bytes_read )
			return true;
		_request_header.push_back(c);
		total_bytes_read += bytes_read;
	}
	_request_header[total_bytes_read] = '\0';
	std::cerr << "\033[32mNEW REQUEST\033[0m" << std::endl;
	//std::cerr << _request_header << std::endl;


    return true;
}

bool Request::parseHeaderRequest(void)
{
	int i;
	int k;
	int j;

	//std::cerr << "-------------------------" << std::endl; 
	//std::cerr << "Start Parsing this request" << std::endl; 
	//std::cerr << _request_header << std::endl; 
	//std::cerr << "-------------------------" << std::endl; 

	// Get Request Method
	i = _request_header.find_first_of(" ", 0);
	_method = _request_header.substr(0, i);
	_m_headers["METHOD: "] = _request_header.substr(0, i);
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
	{
		std::cerr << "Invalid Request Method" << std::endl;
		return false;
	}
	// Get HHTP
	k = _request_header.find_first_of(" ", i + 1);
	_requestFile = _request_header.substr(i + 1, k - i - 1);
	j = _request_header.find_first_of('\r', k + 1);
	_m_headers["HTTP: "] = _request_header.substr(k + 1, j - i - 2);

	// Get rest of the headers
	i = _request_header.find_first_of("\n", j) + 1;
	while (i < _request_header.size())
	{
		k = _request_header.find_first_of(':', i);
		if (k == std::string::npos)
			break;
		j = _request_header.find_first_of("\r\n", k);
		if (j == std::string::npos)
			break;
		std::string key = _request_header.substr(i, k - i + 1);
		//std::cerr << "KEY: " << key << "VALUE:" <<  _request_header.substr(k + 1, j - k) << std::endl;
		_m_headers[_request_header.substr(i, k - i + 1)] = _request_header.substr(k + 1, j - k);
		i = j + 2;
	}

	parseHeaderHelper();

	std::cerr << "*******REQUEST PARSED (MAP)********" << std::endl;
	//std::cout << "-----------------------------------" << std::endl;
	std::map<std::string, std::string>::iterator it;
   	for (it = _m_headers.begin(); it != _m_headers.end(); ++it) {
	//   	std::cerr << it->first << it->second << std::endl;
	}
	//print map content

    return true;
}
void Request::parseHeaderHelper()
{
	std::string host;
	//??Quitar estp
	if (_m_headers.count("Host:"))
	{
		std::cerr << "HHHHOO\n";
	}
	if (_m_headers.count("Host:"))
	{
		host = _m_headers.at("Host:");

		std::vector<std::string> v_host;
		splitString(host, ":", v_host);
	
		//Get ServerName 
 
		_serverName = v_host[0];
		std::cerr << "SERVER NAMMMME"  <<_serverName << "\n\n";

		// Get Port
		_port = std::atoi(v_host[1].c_str());
	}

	// Get Content-Lenght 
	if (_m_headers.count("Content-Length:"))
		_content_length = atoi(_m_headers.at("Content-Length:").c_str());

	// Get Is Chunked
	if (_m_headers.find("Transfer-Encoding:") != _m_headers.end() &&
			   _m_headers["Transfer-Encoding:"] == "chunked")
	{
		_is_chunked = true;
	}
}

bool Request::parseBodyRequest(int client_fd)
{

	// read the request body into a buffer
	if (_is_chunked)
	{
		parseChunkedBody(client_fd);
	}
	else
	{
		parseNotChunkedBody(client_fd);
	}
	
	//std::cout << "REQUEST BODY" << std::endl;
	//std::cout << "------------" << std::endl;
	//std::cout << _request_body << std::endl;
    return true;
}
bool Request::parseNotChunkedBody(int client_fd)
{
	char buffer[20000];
	int bytes_left = _content_length;
	while (bytes_left > 0)
	{
		int bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
		// handle error
		if (bytes_read < 0)
		{
			std::cerr << "Error receiving dataAA" << std::endl;
			return false;
		}
		else
		{
			//std::cout << "buffer:\n";
			//std::cout << buffer << std::endl;
			_request_body.append(buffer, bytes_read);
			bytes_left -= bytes_read;
		}
	}
	_request_body[_content_length] = '\0';
    return true;
}
bool Request::parseChunkedBody(int client_fd) 
{
	char buffer[1024];
	int bytes_left = 0;
	 std::cerr << "EparseChunked_1" << std::endl;

	while (true)
	{
		// read the chunk size
		std::string chunk_size_str;
		int chunk_size;
		while (true) {
			int bytes_read = recv(client_fd, buffer, 1, 0);
			// handle error
			if (bytes_read < 0)
			{
				std::cerr << "Error receiving data Chunked_1" << std::endl;
				return false;
			}
			if (buffer[0] == '\r') {
				break;
			}
			chunk_size_str += buffer[0];
		}
		chunk_size = strtol(chunk_size_str.c_str(), NULL, 16);

		if (chunk_size == 0) {
			// end of chunked data
			break;
		}

		// read the chunk data
		bytes_left = chunk_size;
		while (bytes_left > 0) {
			int bytes_read = recv(client_fd, buffer, std::min(bytes_left, 1024), 0);
			// handle error
			if (bytes_read < 0)
			{
				std::cerr << "Error receiving data Chunked_2" << std::endl;
				return false;
			}

			_request_body.append(buffer, bytes_read);
			bytes_left -= bytes_read;
		}

		// skip the chunk extension and trailing CRLF
		while (true)
		{
			int bytes_read = recv(client_fd, buffer, 1, 0);
			// handle error
			if (bytes_read < 0)
			{
				std::cerr << "Error receiving data Chunked_3"  << std::endl;
				return false;
			}
			if (buffer[0] == '\n') {
				break;
			}
		}
	}
	return true;

}




int Request::getClientFd(void) const
{
	return _client_fd;
}

std::string Request::getMethod(void) const
{
	return _method;
}

std::string Request::getRequestFile(void) const
{
	return _requestFile;
}

std::map<std::string, std::string> Request::getHeaders(void) const
{
	return _m_headers;
}

size_t Request::getContentLength(void) const
{
	return _content_length;
}

std::string Request::getBody(void) const
{
	return _request_body;
}


int Request::getPort() const
{
    return _port;
}

std::string		Request::getServerName() const
{
	return (this->_serverName);
}

std::string Request::getHeader(std::string const &name) const
{
	if (_m_headers.find(name) != _m_headers.end())
		return (_m_headers.at(name));
	return (NULL);
}

std::string Request::getRawBody() const
{
    return _request_raw;
}
bool Request::getIsChunked(void) const
{
    return _is_chunked;
}
std::string Request::getResquestHeaderStr(void) const
{
    return _request_header;
}
void Request::setBody(std::string &a_body)
{
	_request_body = a_body;
}

std::ostream &operator<<(std::ostream &ors, const Request &a_request)
{
   //std::cout << "CLIENT_FD: " << a_request.getClientFd() << std::endl;
   //std::cout << "METHOD: " << a_request.getMethod() << std::endl;
   //std::cout << "RequestFile: " << a_request.getRequestFile() << std::endl;
   ////std::cout << "BODY: " << a_request.getBody() << std::endl;
   ////std::cout << "HEADERS: " << a_request.getHeaders() << std::endl;
   std::map<std::string, std::string> map = a_request.getHeaders();
	std::map<std::string, std::string>::iterator it;
 	std::map<std::string, std::string>::iterator it_end = a_request.getHeaders().end();
	for (it == map.begin(); it != it_end; ++it)
	//	//std::cout << "FIRST" << (*it).first <<  "SECOND" << (*it).second << std::endl;

   //std::cout << "CONTENT LENGHT: " << a_request.getContentLength() << std::endl;
  // //std::cout << "PARSE Chunked Body: " << a_request.parseChunkedBody() << std::endl;
  // //std::cout << "SeverName: " << a_request.getServerName() << std::endl;
   //std::cout << "Port: " << a_request.getPort() << std::endl;

	return ors;
}