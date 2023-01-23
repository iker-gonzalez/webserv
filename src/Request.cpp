#include "Request.hpp"

Request::Request parsing(std::string request)
{
	request = '
	GET / HTTP/1.1
	Host: localhost:4242
	Connection: keep-alive
	sec-ch-ua: "Not_A Brand";v="99", "Google Chrome";v="109", "Chromium";v="109"
	sec-ch-ua-mobile: ?0
	sec-ch-ua-platform: "macOS"
	Upgrade-Insecure-Requests: 1
	User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/109.0.0.0 Safari/537.36
	Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,/;q=0.8,application/signed-exchange;v=b3;q=0.9
	Sec-Fetch-Site: none
	Sec-Fetch-Mode: navigate
	Sec-Fetch-User: ?1
	Sec-Fetch-Dest: document
	Accept-Encoding: gzip, deflate, br
	Accept-Language: es-ES,es;q=0.9
	';
}