#include "../includes/ServerManager.hpp"
#include <iostream>
#include <sys/socket.h>                     // for accept
#include "../includes/Client.hpp"


ServerManager::ServerManager( std::vector<Server> a_v_server) : _v_server(a_v_server),
    _max_socket(0)
{
}

ServerManager::~ServerManager()
{
}

bool ServerManager::serverCore()
{
	std::cout <<  "Waiting for connections..." << std::endl; 
    while (true)
	{
        struct timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
        setupTimeout();
        std::cout << "Set up the file descriptor set for the select function" << std::endl;
        std::cout << " Wait for activity on the socket or timeout" << std::endl;
        if (select(_max_socket + 1, &_read_fds, &_write_fds, NULL, &timeout) < 0)
        {
			std::cerr << "Error in select" << std::endl;
			return false;
		}
        std::cout <<  "!loop over the read_fds" << std::endl;
        for (int i = 0; i < _max_socket; i++)
        {
            // New connection
            if (FD_ISSET(i, &_read_fds) && _m_fd_server.find(i) != _m_fd_server.end()) 
            {
                if (!acceptNewConnection(_m_fd_server.at(i)))
		        	return false;
            }
          // else if (FD_ISSET(i, &_read_fds) && _m_fd_client.find(i) != _m_fd_client.end()) 
          // {
          //     if (!readRequest(_m_fd_server.at(i)))
		  //     	return false;
          // }
          // else if (FD_ISSET(i, &_write_fds))
          // {

		  //     if (!handleConnection())
		//        return false;

          // }
            }
	}
    return true;
}

bool ServerManager::setupServers()
{
    // Initialize my current set
	FD_ZERO(&_read_fds);
	FD_ZERO(&_write_fds);
    std::cout  << "FD_SETSIZE = %d\n" << FD_SETSIZE << std::endl;

    for (unsigned int i = 0; i < _v_server.size(); i++)
    {
        if (!_v_server[i].setupServer())
            return false;
        if (!_v_server[i].listenConnections())
            return false;
        FD_SET(_v_server[i].getListenFd(), &_read_fds);
        if (_v_server[i].getListenFd() > _max_socket)
            _max_socket += 1;
        _m_fd_server[i] = _v_server[i];
    }
    return false;
}

void ServerManager::setupTimeout()
{
    struct timeval struc_time;
    
    struc_time.tv_sec = 5;
    struc_time.tv_usec = 0;

}

bool ServerManager::handleConnection()
{
    return false;
}

bool ServerManager::acceptNewConnection(Server &a_m_server)
{
    int client_sock = accept(a_m_server.getListenFd(), NULL, NULL);
	if (client_sock < 0) 
    {
		// Unable to accept new connections, continue with the loop
	    //if (errno == EAGAIN || errno == EWOULDBLOCK) {
		//		continue;
        std::cerr << "Error accepting connection" << std::endl;
		return false;
    }
    else
    {
        Client new_client(client_sock);

        //Remove fd from _read_fds and add to _write_fds
      // _m_fd_server.erase(a_m_server.getListenFd());
        _m_fd_client[client_sock] = new_client;
        FD_CLR(a_m_server.getListenFd(), &_read_fds);
        FD_SET(a_m_server.getListenFd(), &_write_fds);
    }


    return true;
}
