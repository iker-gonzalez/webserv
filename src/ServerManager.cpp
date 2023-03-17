#include "../includes/ServerManager.hpp"
#include "../includes/Request.hpp"
#include <iostream>
#include <sys/socket.h>                     // for accept, recv
#include "../includes/Client.hpp"
#include <fcntl.h>					        // for fcntl
#include <netinet/in.h>                     // for sockaddr_in 

#include "../includes/CGI.hpp"
#include <sys/types.h>
#include <sys/wait.h>

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
	//std::cout <<  "Max." << _max_socket << std::endl; 
	struct timeval timeout;
	while (true)
	{

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		read_fds = _read_fds;
		write_fds = _write_fds;
		setupTimeout();
		// Set up the file descriptor set for the select function
		 //Wait for activity on the socket or timeout
		if (select(_max_socket, &read_fds, &write_fds, NULL, &timeout) < 0)
		{
			std::cerr << "Error in select" << std::endl;
			return false;
		}
        //loop over the read_fds"
        for (int i = 0; i < _max_socket ; ++i)
        {
            // New connection
            if (FD_ISSET(i, &read_fds) && _m_fd_server.find(i) != _m_fd_server.end())
            {
                if (!acceptNewConnection(_m_fd_server.at(i)))
		            return false;
            }
            else if (FD_ISSET(i, &read_fds) && _m_fd_client.find(i) != _m_fd_client.end()) 
            {
            //   readRequest(_m_fd_client.at(i));
               if (!readRequest(_m_fd_client.at(i)))
		       	return false;
            }
            else if (FD_ISSET(i, &write_fds))
            {
                //Client waiting_client  = _m_fd_client[i];
				//bool isCGI = waiting_client.getIsCGI();
                //if (isCGI && FD_ISSET(waiting_client.response.getCGIResponse().pipe_in[1], &write_fds) )
                //    sendCGIResponse(waiting_client);
				//else if (isCGI && FD_ISSET(waiting_client.response.getCGIResponse().pipe_out[0], &read_fds))
				//{
				//	readCGIResponse(waiting_client);
				//}
		        if (!sendResponse(i, _m_fd_client[i]))
		            return false;
                //std::cout << "MANDAMOS???????" << std::endl;
            

            }
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
		//std::cout << "\033[1;31mNew Server. Port:" << _v_server[i].getPort() << ". Address: "<<  _v_server[i].getAddress() << "\033[0m\n" << std::endl;
	}
	return true;
}

void ServerManager::setupTimeout()
{
	struct timeval struc_time;
	
	struc_time.tv_sec = 5;
	struc_time.tv_usec = 0;

}

bool ServerManager::sendResponse(int fdToSend, Client &ar_client)
{
	int bytes_sent;

	std::string response_content = ar_client.response.getResponseContent();
	std::cout << "response content:\n" << response_content << std::endl;
	std::cout << "\033[38;2;255;165;0m_status code\033[0m: " << ar_client.response.getStatusCode() << std::endl;

	bytes_sent = write(fdToSend, ar_client.response.getResponseContent().c_str(), ar_client.response.getResponseContent().length());
	closeFd(fdToSend);
	//if (bytes_sent < 0 )
	//	return false;
	//else if (bytes_sent == 0 || bytes_sent == ar_client.request.getContentLength())
    //{
	//	if (ar_client.getIsCGI())
	//	{
    //        std:: cerr << "Client " << ar_client.getClientFd() << " Connection Closed." << std::endl;
    //        closeFd(fdToSend);
	//	}
	//	else
	//	{
	//		removeFdSet(fdToSend, _write_fds);
    //        addFdSet(fdToSend, _read_fds);
	//	}
    //}
	//! meter condicion KEEP-ALIVE
	//int bytes_sent = send(fdToSend, _s_buffer.c_str(),atoi(request.getHeaders()["Content-length"].c_str()), 0);
	////std::cout << "bytes sent:" << bytes_sent << std::endl;
	return true;
}

void ServerManager::closeFd(const int fd_to_close)
{
	if (FD_ISSET(fd_to_close, &_read_fds))
		removeFdSet(fd_to_close, _read_fds);
	else 
		removeFdSet(fd_to_close, _write_fds);
	close(fd_to_close);   
	if (_m_fd_server.count(fd_to_close))
		_m_fd_server.erase(fd_to_close);
	else
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



bool ServerManager::sendCGIResponse(Client &a_client)
{
	removeFdSet(a_client.response.getCGIResponse().pipe_out[0], _write_fds);
	close(a_client.response.getCGIResponse().pipe_in[1]);
    close(a_client.response.getCGIResponse().pipe_out[1]);
    return true;
}

bool ServerManager::readCGIResponse(Client &a_client)
{
	char    buffer[30000 * 2];
    int     bytes_read = 0;
    bytes_read = read(a_client.response.getCGIResponse().pipe_out[0], buffer, 30000* 2);
	
	removeFdSet(a_client.response.getCGIResponse().pipe_out[0], _read_fds);
	close(a_client.response.getCGIResponse().pipe_in[0]);
    close(a_client.response.getCGIResponse().pipe_out[0]);
	int status;
	//waitpid(a_client.response.getCGIResponse().getPID(), &status, 0);
	waitpid(-1, &status, 0);
	if(WEXITSTATUS(status) != 0)
	{
		std::cout << "EROOOOR" << std::endl;
		//c.response.setErrorResponse(502);
	}
    a_client.setIsCGI(false);
    return true;
}
bool ServerManager::acceptNewConnection(Server &a_m_server)
{
	//! OP1
	//struct sockaddr_in client_address;
	//long  client_address_size = sizeof(client_address);
	//int client_sock = accept(a_m_server.getListenFd(), (struct sockaddr *)&client_address,
	 //(socklen_t*)&client_address_size);

	 //! OP2
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

		std::cout << "\033[1;31mNew Client. FD:" << client_sock << "\033[0m\n" << std::endl;
		//! TODO Añadir información del servidor al que esta enviado la petición
		Client new_client(a_m_server, client_sock);

		// Set the new socket to non-blocking mode
		/*
			The program sets the new socket to non-blocking mode using the fcntl function and it adds
			the new socket to the read_fds set.
		*/
		 //! OP1
	    fcntl(client_sock, F_SETFL, O_NONBLOCK) ;
	    int flags = fcntl(client_sock, F_GETFL, 0);
	    fcntl(client_sock, F_SETFL, O_NONBLOCK);
		//! OP2
		//int flags = fcntl(client_sock, F_GETFL, 0);
		//fcntl(client_sock, F_SETFL, flags | O_NONBLOCK);

		//Remove fd to _write_fds
		addFdSet(client_sock, _read_fds);
		if (_m_fd_client.count(client_sock) != 0)
		{
			std::cerr << "ERASEEEE" << std::endl;
			_m_fd_client.erase(client_sock);

		}
		_m_fd_client[client_sock] = new_client;

	  //  readRequest(new_client);

	}
	return true;

}
bool ServerManager::readRequest(Client &a_client)
{
	char buffer[1024];
	
	int bytes_received = recv(a_client.getClientFd(), buffer, sizeof(buffer), 0);
	if (bytes_received < 0) 
	{
		std::cerr << "Error receiving data" << std::endl;
		return false;
	}
	std::cerr << "\033[32mNEW REQUEST PARSED\033[0m" << std::endl;
	_s_buffer = buffer;

	std::cerr << _s_buffer << std::endl;
	if (!a_client.request.parseRequest(_s_buffer))
	{
	removeFdSet(a_client.getClientFd() ,_read_fds);
		return true;

	}
	assignServerToClient(a_client);

    std::cout << "\x1B[36mRequest Recived From Socket" << a_client.getClientFd() << "Method = "<< a_client.request.getMethod() <<"\033[0m\n" << std::endl;
//>:cerr << "REQUEST" << a_client.request << std::endl;
	
	a_client.buildResponse();
	if (a_client.response.isCGIResponse())
	{
		//std::cout << "--------CGI BODY ----- " << std::endl;
		//std::cout << a_client.response.getResponseContent() << std::endl;
		//addFdSet(a_client.response.getCGIResponse().pipe_in[1],  _write_fds);
        //addFdSet(a_client.response.getCGIResponse().pipe_out[0],  _read_fds);

	}
	removeFdSet(a_client.getClientFd() ,_read_fds);
	addFdSet(a_client.getClientFd(), _write_fds);
	return true;
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
	//std::cout << "\033[1;31m" << "REMOVEE" << "\033[0m\n" << std::endl;
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
