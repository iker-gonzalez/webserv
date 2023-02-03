#include "../includes/Request.hpp"
#include <string>

int Request::requestParsing(std::string& request)
{

	int i;
	int k;
	int j;

		std::string request = "GET / HTTP/1.1\r\nHost: localhost:4242\r\nConnection: keep-alive\r\nsec-ch-ua: \"Not_A Brand\";v=\"99\", \"Google Chrome\";v=\"109\", \"Chromium\";v=\"109\"\r\nsec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: \"macOS\"\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,/;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: es-ES,es;q=0.9";
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
