#include "../includes/Client.hpp"


Client::Client(int a_fd) : _client_fd(a_fd)
{
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
