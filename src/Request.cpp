#include "../includes/Request.hpp"
#include "../includes/Utils.hpp"
#include <string>
#include <cstdlib>

Request::Request()
{
	_content_length = 0;
	_client_fd = 0;
	_method = "";
	_requestFile = "";
	_content_length = 0;
}
Request::~Request()
{
	
}

int Request::parseHeaders(std::string& request)
{

	int i;
	int k;
	int j;

	std::cout << "****REQUEST NOT PARSED (STRING)****" << std::endl;
	std::cout << "-----------------------------------" << std::endl;
	std::cout << request << std::endl;
	std::cout << "-----------------------------------" << std::endl;
	i = request.find_first_of(" ", 0);
	_method = request.substr(0, i);
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
		return false;
	k = request.find_first_of(" ", i + 1);
	_requestFile = request.substr(i + 1, k - i - 1);
	j = request.find_first_of('\r', k + 1);
	_m_headers["HTTP: "] = request.substr(k + 1, j - i - 2);
	i = request.find_first_of("\n", j) + 1;
	while (i < request.size())
	{
		k = request.find_first_of(':', i);
		if (k == std::string::npos)
			break;
		j = request.find_first_of("\r\n", k);
		if (j == std::string::npos)
			break;
		_m_headers[request.substr(i, k - i + 1)] = request.substr(k + 1, j - k);
		i = j + 2;
	}

	//Get ServerName and Port to which the request was sent
	std::string host = _m_headers ["Host:"];
	std::vector<std::string> v_host;
	splitString(host, ":", v_host);
	_serverName = v_host[0];
	std::cout << "port 1:" << v_host[0] << std::endl;
	_port = std::atoi(v_host[0].c_str());

	//print map content
	std::cout << "*******REQUEST PARSED (MAP)********" << std::endl;
	std::cout << "-----------------------------------" << std::endl;
	std::map<std::string, std::string>::iterator it;
   	for (it = _m_headers.begin(); it != _m_headers.end(); ++it) {
	   	std::cout << it->first << it->second << std::endl;
   }
	return 0;
}

	
int	Request::parseBody() 
{
	// parse the content length or check if chunked encoding is used
	int content_length = -1;
	bool is_chunked = false;
	if (_m_headers.find("Content-Length") != _m_headers.end())
	{
		content_length = atoi(_m_headers["Content-Length"].c_str());
	}
	else if (_m_headers.find("Transfer-Encoding") != _m_headers.end() &&
			   _m_headers["Transfer-Encoding"] == "chunked")
	{
		is_chunked = true;
	}

	// read the request body into a buffer
	std::string body_str;
	if (is_chunked)
	{
		body_str = parseChunkedBody();
	}
	else 
	{
		char buffer[1024];
		int bytes_left = content_length;
		std::cout << "content-length:" << content_length << std::endl;
		while (bytes_left > 0)
		{
			int bytes_read = recv(getClientFd(), buffer, std::min(bytes_left, 1024), 0);
			// handle error
			if (bytes_read < 0)
			{
				std::cerr << "Error receiving data" << std::endl;
				return (1);
			}
			else
			{
				body_str.append(buffer, bytes_read);
				bytes_left -= bytes_read;
			}
		}
	}
	
	// store the request body in the member variable
	_request_body = body_str;
	std::cout << "REQUEST BODY" << std::endl;
	std::cout << "------------" << std::endl;
	std::cout << _request_body << std::endl;
	return 0;
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

std::string Request::parseChunkedBody()
{
	std::string body_str;
	char buffer[1024];
	int bytes_left = 0;

	while (true) {
		// read the chunk size
		std::string chunk_size_str;
		int chunk_size;
		while (true) {
			int bytes_read = recv(getClientFd(), buffer, 1, 0);
			// handle error
			if (bytes_read < 0)
			{
				std::cerr << "Error receiving data" << std::endl;
				return "";
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
			int bytes_read = recv(getClientFd(), buffer, std::min(bytes_left, 1024), 0);
			// handle error
			if (bytes_read < 0)
			{
				std::cerr << "Error receiving data" << std::endl;
				return "";
			}
			body_str.append(buffer, bytes_read);
			bytes_left -= bytes_read;
		}

		// skip the chunk extension and trailing CRLF
		while (true) {
			int bytes_read = recv(getClientFd(), buffer, 1, 0);
			// handle error
			if (bytes_read < 0)
			{
				std::cerr << "Error receiving data" << std::endl;
				return "";
			}
			if (buffer[0] == '\n') {
				break;
			}
		}
	}

	return body_str;
}

void Request::parseRequest(std::string request)
{
	parseHeaders(request);
	parseBody();
}

std::string     Request::getServerName() const
{
    return (this->_serverName);
}
