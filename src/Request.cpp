#include "../includes/Request.hpp"
#include <string>

Request::Request()
{

}
Request::~Request()
{
	
}

int Request::requestParsing(std::string& request)
{

	int i;
	int k;
	int j;

	std::cout << request << std::endl;
	std::cout << "---------------" << std::endl;
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
		j = request.find_first_of("\r\n", k);
		_m_headers[request.substr(i, k - i + 1)] = request.substr(k + 1, j - k);
		if (j == std::string::npos)
			break;
		i = j + 2;
	}
	//print map content
	std::map<std::string, std::string>::iterator it;
   	for (it = _m_headers.begin(); it != _m_headers.end(); ++it) {
       	std::cout << it->first << it->second << std::endl;
   }
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
