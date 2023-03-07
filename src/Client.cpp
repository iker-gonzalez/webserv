#include "../includes/Client.hpp"


Client::Client(Server &serv, int a_fd) : _client_fd(a_fd)
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

void    Client::buildResponse()
{
    response.setRequest(this->request);
    response.buildResponse();
}

void    Client::setServer(Server &serv)
{
    response.setServer(serv);
}
