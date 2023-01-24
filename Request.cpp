#include "Request.hpp"
#include <curl/curl.h>
#include <string>

int main()
{
	std::string method;
	std::string uri;
	std::map<std::string, std::string> headers;

	int i;
	int k;
	int j;

		std::string request = "GET / HTTP/1.1\r\nHost: localhost:4242\r\nConnection: keep-alive\r\nsec-ch-ua: \"Not_A Brand\";v=\"99\", \"Google Chrome\";v=\"109\", \"Chromium\";v=\"109\"\r\nsec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: \"macOS\"\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,/;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nSec-Fetch-Site: none\r\nSec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\nSec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: es-ES,es;q=0.9";
		std::cout << request << std::endl;
		std::cout << "---------------" << std::endl;
		i = request.find_first_of(" ", 0);
		method = request.substr(0, i);
		std::cout << method << std::endl;
		k = request.find_first_of(" ", i + 1);
		uri = request.substr(i + 1, k - i - 1);
		std::cout << uri << std::endl;
		j = request.find_first_of('\r', k + 1);
		headers["HTTP"] = request.substr(k + 1, j - i - 2);
		std::cout << headers["HTTP"] << std::endl;
		std::cout << request.size() << std::endl;
		i = request.find_first_of("\n", j) + 1;
		while (i < request.size())
		{
			k = request.find_first_of(':', i);
			j = request.find_first_of("\r\n", k);
			std::cout << "j:" << j << std::endl;
			headers[request.substr(i, k - 1)] = request.substr(k + 1, j - 1);
			std::cout << headers[request.substr(i, k - 1)];
			std::cout << request.substr(k + 1, j - 1) << std::endl;
			i = j + 1;
			std::cout << "i:" << i << std::endl;
		}

	return 0;
}
