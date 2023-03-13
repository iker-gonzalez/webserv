#ifndef SERVERMANAGER_HPP
#define SERVERMANAGER_HPP

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Response.hpp"

#include <sys/types.h>
#include <unistd.h>

class ServerManager 
{
public:
	ServerManager(std::vector<Server> a_v_server);
	~ServerManager();

	bool serverCore();

    //Check maximun wait time
    void setupTimeout();

    // Creates a socket with the specifies information
    bool setupServers(); 

    // Server core. Infinitive loop
    bool acceptNewConnection(Server &a_m_server);
    bool readRequest(Client &a_client);
    bool sendResponse(int fdToSend, Client& ar_client);
    bool sendCGIResponse(Client &a_client);
    bool readCGIResponse(Client &a_client);

    // FSET new fd in _read_fds/_write_fds and uppdate _max_socket
    void addFdSet(int new_fd, fd_set &a_fds_set);
    void removeFdSet(int remove_fd, fd_set &a_fds_set);

    // Close before exiting  
    void closeServerSocket(void);

    // Assign server config to a client
    void    assignServerToClient(Client &client);
    // Close fd and remove from read_ or write set
    void closeFd(const int fd_to_close);

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
    std::string    _s_buffer;


    // Map contains each server with its file descriptor
    std::map<int, Server>   _m_fd_server;
    // Map contains each client with its file descriptor
    std::map<int, Client>   _m_fd_client;
};

#endif