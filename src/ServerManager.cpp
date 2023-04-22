#include "../includes/ServerManager.hpp"
#include <iostream>
#include <sys/socket.h>                     // for accept, recv
#include <fcntl.h>					        // for fcntl
#include <netinet/in.h>                     // for sockaddr_in 

#include "../includes/CGI.hpp"
#include <sys/wait.h>

#define MAX_TIMEOUT 10 // Time in seconds before client get kicked out if no data was sent.

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
		setupTimeout();
		// Set up the file descriptor set for the select function
		// Wait for activity on the socket or timeout
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
				std::cerr << "readRequest I:"<< i << std::endl;
				std::cerr << " readRequestISISEL:"<< _m_fd_client[i].getIsCGI()  << std::endl;

               readRequest(i, _m_fd_client.at(i));
              //if (!readRequest(i, _m_fd_client.at(i)))
		      //	return false;
            }


			else if (_m_fd_client[i].getIsCGI() == true && _m_fd_client[i].request.getMethod() == "POST" &&
				FD_ISSET(_m_fd_client[i].response.getCGIResponse().pipe_in[1], &write_fds)) 
            {
				std::cerr << "sendCGIResponse I:"<< i << std::endl;
				
				sendCGIResponse(_m_fd_client[i]);
            }
			else if(_m_fd_client[i].getIsCGI() == 1 && FD_ISSET(_m_fd_client[i].response.getCGIResponse().pipe_out[0], &read_fds))
			{
				std::cerr << "readCGIResponse:"<< i << "PEP_OIT" <<  std::endl;
				std::cerr << "ISCGISEL:"<< _m_fd_client[i].getIsCGI() <<_m_fd_client[i].response.getCGIResponse().pipe_out[0] << std::endl;
				//std::cerr <<_m_fd_client[i].response.getCGIResponse().pipe_out[0] <<"there is a CGI READSELE\n "; 
					readCGIResponse(_m_fd_client[i]);
			}
			else if (FD_ISSET(i, &write_fds) && !_m_fd_client[i].getIsCGI()  && _m_fd_client.find(i) != _m_fd_client.end())
			{
		        	sendResponse(i, _m_fd_client[i]);

			}

			//checkTimeout();
				
			//	//std::cerr << "sendResponse:"<< i << std::endl;
//
			//	//else if (!_m_fd_client[i].getIsCGI())
        }
	}
	closeServerSocket();
	return true;
}
           // else if (FD_ISSET(i, &write_set_cpy) && _clients_map.count(i))
           // {
           //     int cgi_state = _m_fd_client[i].response.getCgiState(); // 0->NoCGI 1->CGI write/read to/from script 2-CGI read/write done
           //     if (cgi_state == 1 && FD_ISSET(_m_fd_client[i].response.getCGIResponse.pipe_in[1], &write_set_cpy))
           //         sendCGIResponse(_m_fd_client[i]);
           //     else if (cgi_state == 1 && FD_ISSET(_m_fd_client[i].response._cgi_obj.pipe_out[0], &recv_set_cpy))
           //         readCgiResponse(_m_fd_client[i]);
           //     else if ((cgi_state == 0 || cgi_state == 2)  && FD_ISSET(i, &write_set_cpy))
           //         sendResponse(i, _m_fd_client[i]);
           // }
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
	std::cerr << "SEND RESPONSE: " << ar_client.response.getStatusCode() << std::endl;
	//std::cerr << "\033[38;2;255;165;0m_status code\033[0m: " << ar_client.response.getStatusCode() << std::endl;
	//std::cerr << "\033[38;2;255;165;0mFD\033[0m: " << fdToSend << std::endl;

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

bool ServerManager::sendCGIResponse(Client &ar_client)
{

	int pipeToSent = ar_client.response.getCGIResponse().pipe_in[1];
	std::string bodyToSent = ar_client.request.getBody();

	//close(ar_client.response.getCGIResponse().pipe_in[0]);
	int bytes_sent = 0;
	std::cerr << "\033[1;31msendCGIResponse FD:" << pipeToSent << "Body:" <<bodyToSent << "\033[0m\n" << std::endl;
	bytes_sent = write(pipeToSent, bodyToSent.c_str(), bodyToSent.size());
	
	
	if (bytes_sent < 0)
	{
	   	closeFd(pipeToSent);
		std::cerr << "PIPE INFO:\n" << bodyToSent.c_str() << std::endl;
		//close(ar_client.response.getCGIResponse().pipe_in[0]);
		
		return false;

	}
	//else if (bytes_sent == 0 || bytes_sent == bodyToSent.length())
	else
	{
	   	closeFd(pipeToSent);
		std::cerr << "FISICH SENDING PIPE INFO:\n" << bodyToSent.c_str() << std::endl;
		//ar_client.setIsCGI(1);
		
		return true;
	}
	//else
	//{
	//	std::cerr << "FNOT FISNHES\n" << bodyToSent.c_str() << std::endl;
//
	//	bodyToSent = bodyToSent.substr(bytes_sent);
	//	ar_client.request.setBody(bodyToSent);
	//}
	//ar_client.setIsCGI(1);
	//removeFdSet(pipeToSent, _write_fds);
    return true;
}

void ServerManager::closeFd(const int fd_to_close)
{
	if (fd_to_close > 100)
		exit(6);
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

		std::cerr << "\033[1;31mNew Client. FD:" << client_sock << "\033[0m\n" << std::endl;
		//! TODO Añadir información del servidor al que esta enviado la petición
		Client new_client(a_m_server, client_sock);

		// Set the new socket to non-blocking mode
		/*
			The program sets the new socket to non-blocking mode using the fcntl function and it adds
			the new socket to the read_fds set.
		*/
		 //! OP1
	    fcntl(client_sock, F_SETFL, O_NONBLOCK) ;
	    //int flags = fcntl(client_sock, F_GETFL, 0); //?? En el subject no permiten utilizar otro flag
	    //fcntl(client_sock, F_SETFL, O_NONBLOCK);
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

		//std::cerr << "\033[1;31mNew Client2. FD:" << client_sock << "\033[0m\n" << std::endl;
		_m_fd_client[client_sock] = new_client;

	  //  readRequest(new_client);

	}
	return true;

}
bool ServerManager::readRequest(int fd, Client &a_client)
{
	
    std::cerr << "\x1B[36mPARSE:"  <<fd <<std::endl;
	bool status = a_client.request.parseRequest(fd);
	if (!status || a_client.request.getResquestHeaderStr().empty())
	{
		//if (a_client.request.getHeade())
		//removeFdSet(a_client.getClientFd() ,_read_fds);
		if (status)
		{
			closeFd(a_client.getClientFd());
			std::cerr << "EMPTY MESSAGE\n";
		}
		else
			std::cerr << "Error parsing request\n";
		return status;
	}

	//??Mirar si se puede quitar esto
	//assignServerToClient(a_client);

    std::cerr << "\x1B[36mRequest Recived From Socket" << a_client.getClientFd() << "Method = "<< a_client.request.getMethod() <<"\033[0m\n" << std::endl;
	//>:cerr << "REQUEST" << a_client.request << std::endl;
	
	// Build response
	a_client.buildResponse();

	// Check if there is a CGI Request and add fd to write and read arrays.
	if (a_client.getIsCGI())
    {
		std::cerr <<a_client.getIsCGI()<< "there is a CGI Request and\n ";
		//if (a_client.request.getMethod() == "POST" ||a_client.request.getMethod() == "DELETE") 
		if (!a_client.request.getBody().empty()) 
		{
        	addFdSet(a_client.response.getCGIResponse().pipe_in[1], _write_fds);
			std::cerr << a_client.response.getCGIResponse().pipe_in[1]<< "there is a CGI WRITE\n "; 
		}

		std::cerr <<a_client.response.getCGIResponse().pipe_out[0] <<"there is a CGI READ\n "; 
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
	std::cerr << "readCGIResponse FD:" << pipeToRead  << std::endl;
	//std::cerr << "\033[1;readCGIResponse FD:" << pipeToRead << "\033[0m\n" << std::endl;
	std::string bodyToSent = "";
    char buffer[1024];
	int  readBytes;
	int  readBytesTotal = 0;
	while ((readBytes = read(pipeToRead, buffer, 1024)) > 0)
	{
		bodyToSent.append(buffer, readBytes);
        readBytesTotal +=  readBytes;
    	if (readBytes == -1)
		{
			std::cerr << "Couldn't read from CGI " << ": " << strerror(errno) << std::endl;
			closeFd(pipeToRead);
			a_client.setIsCGI(0);
			return false;
		}
	}
	a_client.response.setResponseContent(bodyToSent); 
	std::cerr << "CGI Response: " << bodyToSent << std::endl;
   	closeFd(pipeToRead);
    if (waitpid(0, NULL, WNOHANG) < 0)
    {
        std::cerr << "CHILD ERROR: " << strerror(errno) << std::endl;
    }
    else{
        std::cerr << "NOT CHILD ERROR: " << std::endl;
    }
	a_client.setIsCGI(0);
	//exit(2);
    return true;
}

void ServerManager::checkTimeout()
{
	for(std::map<int, Client>::iterator it = _m_fd_client.begin() ; it != _m_fd_client.end(); ++it)
    {
        if (time(NULL) - it->second.getTimeOut() > MAX_TIMEOUT && it->second.getIsCGI())
        {
			std::cerr << "Close TIEMOUT" << std::endl;
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
