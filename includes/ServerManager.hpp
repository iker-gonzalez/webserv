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
    bool acceptNewConnection(Server &a_m_server);
    bool readRequest(Client &a_client);
    bool sendResponse(int fdToSend);
    void setupTimeout();

    // FSET new fd in _read_fds/_write_fds and uppdate _max_socket
    void addFdSet(int new_fd, fd_set &a_fds_set);
    void removeFdSet(int remove_fd, fd_set &a_fds_set);

private:
    // Vector with all servers of config files 
	std::vector<Server>     _v_server;

    int                     _max_socket;

    // File descriptor to read and to write
    fd_set        _read_fds;
    fd_set        _write_fds;

    fd_set        _wait_fd_server;
    fd_set        _read_from_client;
    fd_set        _write_to_client;


    // Map contains each server with its file descriptor
    std::map<int, Server>   _m_fd_server;
    // Map contains each client with its file descriptor
    std::map<int, Client>   _m_fd_client;
};

#endif