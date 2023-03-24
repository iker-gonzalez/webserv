#include "../includes/Client.hpp"



Client::Client(Server &serv, int a_fd) : _client_fd(a_fd), _is_CGI(false)
{
    setServer(serv);
}

Client::Client()
{
}
Client::~Client()
{
}

int Client::getClientFd() const
{
    return _client_fd;
}

Server Client::getServer() const
{
    return server;
}

void Client::buildResponse()
{
	////std::cerr << "build Body" << std::endl;

    response.setRequest(this->request);
    response.buildResponse();

}
bool Client::getIsCGI() const
{
    return _is_CGI;
}

void Client::setIsCGI(bool a_is_CGI)
{
    _is_CGI = a_is_CGI;
}

void Client::clear()
{

    Request new_request;
    request = new_request;
	//std::cerr << "_--WRITE BYTE46S" << std::endl;
    Response new_response;
	//std::cerr << "_--WRITE BYTE45S"  << std::endl;
    response = new_response;

}
void Client::setServer(Server &serv)
{
    response.setServer(serv);
}
