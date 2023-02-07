#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"

#include <sys/types.h>
#include <unistd.h>

class ServerManager 
{
public:
	ServerManager(std::vector<Server> a_v_server);
	~ServerManager();


	bool serverCore();

    // Creates a socket with the specifies information
    bool setupServers(); 

    // 
    void setupTimeout();
    bool handleConnection();
    bool acceptNewConnection(Server &a_m_server);

private:
    // Vector with all servers of config files 
	std::vector<Server>     _v_server;

    int                     _max_socket;

    // File descriptor to read and to write
    fd_set        _read_fds;
    fd_set        _write_fds;

    // Map contains each server with its file descriptor
    std::map<int, Server>   _m_fd_server;
    // Map contains each client with its file descriptor
    std::map<int, Client>   _m_fd_client;
};

#endif