#include "../includes/Client.hpp"



Client::Client(Server &serv, int a_fd) : _client_fd(a_fd)
{
    setServer(serv);
    _time_out = time(NULL);
}

Client::Client()
{
    _time_out = time(NULL);
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

void    Client::buildResponse()
{
    response.setRequest(this->request); 
    response.buildResponse();
}
int Client::getIsCGI() const
{
    return response.isCGIResponse();
}

void Client::setIsCGI(int a_is_CGI)
{
    response.setIsCGIResponse(a_is_CGI);
}

void    Client::setServer(Server &serv)
{
    response.setServer(serv);
}

time_t  Client::getTimeOut() const
{
    return _time_out;
}