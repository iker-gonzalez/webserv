#include "../includes/ServerManager.hpp"
#include <iostream>
#include <sys/socket.h>                     // for accept, recv
#include <fcntl.h>					        // for fcntl
#include <netinet/in.h>                     // for sockaddr_in 

#include "../includes/CGI.hpp"
#include <sys/wait.h>

#define MAX_TIMEOUT 30 // Time in seconds before client get kicked out if no data was sent.

ServerManager::ServerManager( std::vector<Server> a_v_server) : _v_server(a_v_server),
	_max_socket(0)
{
}

ServerManager::~ServerManager()
{
}

bool ServerManager::serverCore()
{
	fd_set  read_fds;
	fd_set  write_fds;
	std::cout <<  "Waiting for connections..." << std::endl; 
	struct timeval timeout;
	while (true)
	{
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		read_fds = _read_fds;
		write_fds = _write_fds;
		// Set up the file descriptor set for the select function
		// Wait for activity on the socket or timeout
		if (select(_max_socket, &read_fds, &write_fds, NULL, &timeout) < 0)
		{
			std::cerr << "Error in select" << std::endl;
			return false;
		}
        for (int i = 0; i < _max_socket  ; ++i)
        {
            // New connection
            if (FD_ISSET(i, &read_fds) && _m_fd_server.find(i) != _m_fd_server.end())
            {
                if (!acceptNewConnection(_m_fd_server.at(i)))
		            return false;
            }
            else if (FD_ISSET(i, &read_fds) && _m_fd_client.find(i) != _m_fd_client.end()) 
            {
               readRequest(i, _m_fd_client.at(i));
            }


			else if (_m_fd_client[i].getIsCGI() == true && _m_fd_client[i].request.getMethod() != "GET" &&
				FD_ISSET(_m_fd_client[i].response.getCGIResponse().pipe_in[1], &write_fds)) 
            {
				sendCGIResponse(_m_fd_client[i]);
            }
			else if(_m_fd_client[i].getIsCGI() == 1 && FD_ISSET(_m_fd_client[i].response.getCGIResponse().pipe_out[0], &read_fds))
			{
				readCGIResponse(_m_fd_client[i]);
			}
			else if (FD_ISSET(i, &write_fds) && !_m_fd_client[i].getIsCGI()  && _m_fd_client.find(i) != _m_fd_client.end())
			{
		        	sendResponse(i, _m_fd_client[i]);

			}
			checkTimeout();
        }
	}
	closeServerSocket();
	return true;
}

bool ServerManager::setupServers()
{
	// Initialize my current set
	FD_ZERO(&_read_fds);
	FD_ZERO(&_write_fds);

	for (unsigned int i = 0; i < _v_server.size(); i++)
	{
		if (!_v_server[i].setupServer())
		{
			closeServerSocket();
			return false;
		}
		if (!_v_server[i].listenConnections())
			return false;
		addFdSet(_v_server[i].getListenFd(), _read_fds);

		// Store Server and Fd
		_m_fd_server[_v_server[i].getListenFd()] = _v_server[i];
		std::cerr << "\033[1;31mNew Server. Port:" << _v_server[i].getPort() << ". Address: "<<  _v_server[i].getAddress() << "\033[0m\n" << std::endl;
	}
	return true;
}



bool ServerManager::sendResponse(int fdToSend, Client &ar_client)
{
	int bytes_sent;

	std::string response_content = ar_client.response.getResponseContent();
	bytes_sent = send(fdToSend, ar_client.response.getResponseContent().c_str(), ar_client.response.getResponseContent().length(), 0);
	closeFd(fdToSend);
	if (bytes_sent < 0 )
		return false;
//	else if (bytes_sent == 0 || bytes_sent == ar_client.request.getContentLength())
	else if (bytes_sent == 0)
    {
        closeFd(fdToSend);
		return true;
	}
    closeFd(fdToSend);
	return true;
}

bool ServerManager::sendCGIResponse(Client &ar_client)
{

	int pipeToSent = ar_client.response.getCGIResponse().pipe_in[1];
	std::string bodyToSent = ar_client.request.getBody();
	bodyToSent[bodyToSent.length()] = '\0';
	int bytes_sent = 0;
	bytes_sent = write(pipeToSent, bodyToSent.c_str(), bodyToSent.size());
	if (bytes_sent < 0)
	{
	   	closeFd(pipeToSent);
		return false;

	}
	else if (bytes_sent == 0 || bytes_sent == (int)bodyToSent.length())
	{
	   	closeFd(pipeToSent);
		return true;
	}
    return true;
}

void ServerManager::closeFd(const int fd_to_close)
{
    std:: cerr << "\033[32m " << fd_to_close << " Connection Closed.\033[0m" << std::endl;
	if (FD_ISSET(fd_to_close, &_read_fds))
		removeFdSet(fd_to_close, _read_fds);
	else
		removeFdSet(fd_to_close, _write_fds);

	close(fd_to_close);   
	if (_m_fd_server.count(fd_to_close))
		_m_fd_server.erase(fd_to_close);
	else if (_m_fd_client.count(fd_to_close))
		_m_fd_client.erase(fd_to_close);
	
}

void ServerManager::closeServerSocket(void)
{
	std::vector<Server>::iterator it = _v_server.begin();
	std::vector<Server>::iterator it_end = _v_server.end();
	for (it =  _v_server.begin(); it != it_end; it++)
	{
		close((*it).getListenFd());   
	}
}

bool ServerManager::acceptNewConnection(Server &a_m_server)
{

	int client_sock = accept(a_m_server.getListenFd(), NULL, NULL);
	if (client_sock < 0) 
	{
		// Unable to accept new connections, continue with the loop
		std::cerr << "Error accepting connection" << std::endl;
		return false;
	}
	else
	{

		std::cerr << "\033[1;31mNew Client. FD:" << client_sock << "\033[0m\n" << std::endl;
		Client new_client(a_m_server, client_sock);

		// Set the new socket to non-blocking mode
		/*
			The program sets the new socket to non-blocking mode using the fcntl function and it adds
			the new socket to the read_fds set.
		*/
	    fcntl(client_sock, F_SETFL, O_NONBLOCK) ;

		//Remove fd to _write_fds
		addFdSet(client_sock, _read_fds);
		if (_m_fd_client.count(client_sock) != 0)
		{
			_m_fd_client.erase(client_sock);

		}

		_m_fd_client[client_sock] = new_client;
	}
	return true;

}
bool ServerManager::readRequest(int fd, Client &a_client)
{
	
	bool status = a_client.request.parseRequest(fd);
	if (!status || a_client.request.getResquestHeaderStr().empty())
	{
		if (status)
		{
			closeFd(a_client.getClientFd());
		}
		else
			std::cerr << "Error parsing request\n";
		return status;
	}

    std::cerr << "\x1B[36mRequest Recived From Socket" << a_client.getClientFd() << "Method = "<< a_client.request.getMethod() <<"\033[0m\n" << std::endl;
	
	// Build response
	a_client.buildResponse();

	// Check if there is a CGI Request and add fd to write and read arrays.
	if (a_client.getIsCGI())
    {
		if (!a_client.request.getBody().empty()) 
		{
        	addFdSet(a_client.response.getCGIResponse().pipe_in[1], _write_fds);
		}
        addFdSet(a_client.response.getCGIResponse().pipe_out[0], _read_fds);
    }

	// Change client fd from read to write array
	removeFdSet(a_client.getClientFd() ,_read_fds);
	addFdSet(a_client.getClientFd(), _write_fds);
	return true;
}

bool ServerManager::readCGIResponse(Client &a_client)
{

	int pipeToRead = a_client.response.getCGIResponse().pipe_out[0];
	std::string bodyToSent = "";
    char buffer[1024];
	int  readBytes;
	int  readBytesTotal = 0;
	while ((readBytes = read(pipeToRead, buffer, 1024)) > 0)
	{
    	if (readBytes == -1)
		{
			std::cerr << "Couldn't read from CGI " << std::endl;
			closeFd(pipeToRead);
			a_client.setIsCGI(0);
			return false;
		}
		else if (readBytes == 0)
		{
			break;
		}
		bodyToSent.append(buffer, readBytes);
        readBytesTotal +=  readBytes;
	}
	bodyToSent[bodyToSent.length()] = '\0';
	a_client.response.setResponseContent(bodyToSent); 
   	closeFd(pipeToRead);
    if (waitpid(0, NULL, WNOHANG) < 0)
    {
        std::cerr << "CHILD ERROR" << std::endl;
    }
    else{
        std::cerr << "NOT CHILD ERROR: " << std::endl;
    }
	a_client.setIsCGI(0);
    return true;
}

void ServerManager::checkTimeout()
{
	for(std::map<int, Client>::iterator it = _m_fd_client.begin() ; it != _m_fd_client.end(); ++it)
    {
        if (time(NULL) - it->second.getTimeOut() > MAX_TIMEOUT && it->second.getIsCGI())
        {
            closeFd(it->first);
            return ;
        }
    }
}
void ServerManager::addFdSet(int new_fd, fd_set &a_fds_set)
{
	FD_SET(new_fd, &a_fds_set);
	if (new_fd > _max_socket)
		 _max_socket = new_fd + 1;
}

void ServerManager::removeFdSet(int remove_fd, fd_set &a_fds_set)
{
	FD_CLR(remove_fd, &a_fds_set);
	if (remove_fd == _max_socket)
		 _max_socket -= 1;
}

/* Assign serverconfiguration to a client*/
void    ServerManager::assignServerToClient(Client &client)
{
	for (std::vector<Server>::iterator it = _v_server.begin();
		it != _v_server.end(); ++it)
	{
		if (client.server.getListen() == it->getListen() &&
			client.server.getPort() == it->getPort() &&
			client.request.getServerName() == it->getServerName())
		{
			client.setServer(*it);
			return ;
		}
	}
}
