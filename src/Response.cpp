#include "../includes/Response.hpp"

Response::Response()
{
	_response_content = "";
	_response_body = "";
	_status_code = 0;
}

Response::Response(Request &req) : request(req)
{
	_response_content = "";
	_response_body = "";
	_status_code = 0;
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

int		Response::buildBody()
{
	if (request.getMethod() == "GET")
		readFile();

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
	buildBody();
	setStatusLine();
	setHeaders();
	if (request.getMethod() == "GET")
		_response_content.append(_response_body);
}

//start treating request in handleTarget function