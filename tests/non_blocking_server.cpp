#include <iostream>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include "../includes/Utils.hpp"
#include "../includes/ConfigFile.hpp"
#include "../includes/Server.hpp"

#include <cerrno>

int non_blocking_server(ConfigFile& conf)
{

	//! Create a socket
	/*
		The program starts by creating a socket using the socket function,
		it specifies the address family (AF_INET), the type of socket (SOCK_STREAM)
		and the protocol (0) as parameters.
	*/

		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0) 
		{
			std::cerr << "Error creating socket" << std::endl;
			return 1;
		}


	//! Set the socket to non-blocking mode
	/*
		The program sets the socket to non-blocking mode using the fcntl function,
		it retrieves the current flags using F_GETFL and then sets the O_NONBLOCK flag using F_SETFL
	*/

		int flags = fcntl(sock, F_GETFL, 0);
		fcntl(sock, F_SETFL, flags | O_NONBLOCK);


    //! Bind the socket to an address and port
	/*
		The program binds the socket to an address and port using the bind function,
		it creates a sockaddr_in struct and sets the address family, IP address and port number.
	*/

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		std::string host_string = "127.0.0.1";
		in_addr_t host = inet_addr(host_string.data());
		addr.sin_addr.s_addr = 	host;
		addr.sin_port = htons(8080);
		char buf[INET_ADDRSTRLEN];
    	//inet_ntop(AF_INET, &host, buf, INET_ADDRSTRLEN);
		if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) 
		{
			std::cerr << "Error binding socket" << std::endl;
			return 1;
		}



	std::cout <<  "Start listening for incoming connections" << std::endl; 
	//! Start listening for incoming connections
	/*
		The program starts listening for incoming connections using the listen function,
		it specifies the socket descriptor and the maximum number of connections to be 
		queued up as parameters.
	*/

		if (listen(sock, SOMAXCONN) < 0) 
		{
			std::cerr << "Error listening on socket" << std::endl;
			return 1;
		}

	while (true) 
	{
		std::cout << "Set up the file descriptor set for the select function" << std::endl;
		//! Set up the file descriptor set for the select function
		/*
			The program enters an infinite loop, in which it calls the select function to wait
			for activity on the socket. It creates two file descriptor sets, read_fds and write_fds
			using FD_ZERO, FD_SET, FD_CLR and FD_ISSET respectively.
		*/

			fd_set read_fds, write_fds;
			FD_ZERO(&read_fds);
			FD_ZERO(&write_fds);
			FD_SET(sock, &read_fds);


		//! Set up the timeout for the select function
		/*
			The program sets up a timeout for the select function using the timeval struct,
			it specifies the seconds and microseconds.
		*/

			struct timeval timeout;
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;


		//! Wait for activity on the socket or timeout
		/*
			The program waits for activity on the socket or timeout by calling the select function
			and passing the maximum file descriptor value, the read and write file descriptor sets,
			and the timeout struct as parameters.
		*/

			int nfds = select(sock + 1, &read_fds, &write_fds, NULL, &timeout);
			if (nfds < 0) {
				std::cerr << "Error in select" << std::endl;
				return 1;
			}


			if (FD_ISSET(sock, &read_fds)) {
			//! Accept an incoming connection
			std::cout << "Accept an incoming connection" <<std::endl;
			/*
				The program checks if the socket is set in the read_fds set, if it is, it calls the 
				accept function to accept a new incoming connection.
			*/

				int client_sock = accept(sock, NULL, NULL);
				if (client_sock < 0) 
				{
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// Unable to accept new connections, continue with the loop
				continue;
				} else {
				std::cerr << "Error accepting connection" << std::endl;
				return 1;
				}
				}


			//! Set the new socket to non-blocking mode
			/*
				The program sets the new socket to non-blocking mode using the fcntl function and it adds
				the new socket to the read_fds set.
			*/

				flags = fcntl(client_sock, F_GETFL, 0);
				fcntl(client_sock, F_SETFL, flags | O_NONBLOCK);
				//add the client_sock to the read_fds
				FD_SET(client_sock, &read_fds);
				}


			std::cout <<  "!loop over the read_fds" << std::endl;
			/*
				The program loops over the read_fds set and checks which descriptor is set. If the descriptor is set,
				the program calls the recv function to receive data from the client.
			*/
			for(int i = sock + 1; i < FD_SETSIZE; i++) 
			{
				if(FD_ISSET(i, &read_fds)) 
				{
					// Receive data from the client
					char buffer[1024];
					int bytes_received = recv(i, buffer, sizeof(buffer), 0);
					if (bytes_received < 0) 
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK) 
						{
							std::cerr << " Unable to receive data, continue with the loop" << std::endl;

							// Unable to receive data, continue with the loop
							continue;
						} 
						else 
						{
							std::cerr << "Error receiving data" << std::endl;
							return 1;
						}
					}
					//! Echo the data back to the client
					/*
						The program echoes the data back to the client using the send function.
					*/
				std::string s_buffer = buffer;
					std::cout << "buffer:" << s_buffer << std::endl;
	//				int bytes_sent = send(i, buffer, bytes_received, 0);
	//				if (bytes_sent < 0) 
	//				{
	//					if (errno == EAGAIN || errno == EWOULDBLOCK) 
	//					{
	//						// Unable to send data, continue with the loop
	//						continue;
	//					} 
	//					else 
	//					{
	//						std::cerr << "Error sending data" << std::endl;
	//						return 1;
	//					}
	//				}
	//				//! Close the client socket if all data sent
	//				/*
	//					The program checks if all the data has been sent, if so it closes the client socket.
	//				*/
//
	//				if(bytes_sent == bytes_received)
					close(i);
				}
			}
	//! Infinite loop
	/*
		* The program repeats indefinitely, until the program is closed (Ctrl + Z).
	*/
	}

	//! Close the server socket
	/*
		Finally, the program closes the server socket and exits.
	*/
	close(sock);
	return 0;
}

//?Additional comments */
/*
	Please note that this is a very basic example of a server, and there is no error
	handling or any way to stop the program other than closing it. In a real-world scenario,
	you would need to add a way to gracefully stop the server, handle errors and unexpected situations,
	and also take care of security issues.

	Moreover, in our program the use of ERRNO is forbidden and here HTTP request are not actually executed.
*/


