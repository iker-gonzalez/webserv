#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "../includes/Client.hpp"

#include <sys/types.h>
#include <unistd.h>

class ServerManager 
{
public:
	ServerManager(std::vector<Server> a_v_server);
	~ServerManager();

	bool serverCore();

    //Check maximun wait time
    void setupTimeout(); //?? Implementarlo

    // Creates a socket with the specifies information
    bool setupServers(); 

    // Server core. Infinitive loop
    bool acceptNewConnection(Server &a_m_server);
    bool readRequest(int fd, Client &a_client);
    bool sendResponse(int fdToSend, Client& ar_client);


    // FSET new fd in _read_fds/_write_fds and uppdate _max_socket
    void addFdSet(int new_fd, fd_set &a_fds_set);
    void removeFdSet(int remove_fd, fd_set &a_fds_set);

    // Close before exiting  
    void closeServerSocket(void);
    // Close fd and remove from read_ or write set
    void closeFd(const int fd_to_close);

    // Assign server config to a client
    void    assignServerToClient(Client &client);

private:
    // Vector with all servers of config files 
	std::vector<Server>     _v_server; //?? Mirar si se puede quitar y dejar solo el mapa

    int                     _max_socket;

    // File descriptor to read and to write
    fd_set        _read_fds;
    fd_set        _write_fds;

    fd_set        _read_from_client;
    fd_set        _write_to_client;

    // Map contains each server with its file descriptor
    std::map<int, Server>   _m_fd_server;
    // Map contains each client with its file descriptor
    std::map<int, Client>   _m_fd_client;
};

#endif