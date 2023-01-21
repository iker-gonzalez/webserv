#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{

	//! Create the socket
	/*
		The first step is to create a socket using the socket() function. 
		This function takes three arguments: the address family (AF_INET for IPv4), 
		the type of socket (SOCK_STREAM for TCP), and the protocol (0 for the default protocol, which is usually TCP). 
		The socket() function returns a file descriptor for the new socket, which is stored in the sock variable.
	*/

		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0) {
			std::cerr << "Failed to create socket" << std::endl;
			return 1;
		}


	//! Set up the server address
	/*
		Next, the code sets up the server address using the sockaddr_in structure.
		This structure holds information about an IPv4 address, such as the IP address
		and the port number. The memset() function is used to clear the memory of the
		server_addr variable and set the address family to AF_INET, the port number to 80,
		and the IP address to "93.184.216.34" (which is the IP address of example.com website)
	*/

		struct sockaddr_in server_addr;
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(80);
		inet_pton(AF_INET, "93.184.216.34", &server_addr.sin_addr);


	//! Connect to the server
	/*
		The connect() function is called to establish a connection to the server. 
		It takes three arguments: the file descriptor of the socket (sock), a pointer
		to the sockaddr_in structure that holds the server address (&server_addr), and
		the size of the sockaddr_in structure (sizeof(server_addr)).
		If the connect() function call is successful, it returns 0, otherwise, it returns -1
		and the program will print "Failed to connect to server" message.
	*/

		if (connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
			std::cerr << "Failed to connect to server" << std::endl;
			return 1;
	}


	//! Send the GET request
	/*
		Next, the code constructs an HTTP GET request using a string variable named request,
		it contains the HTTP GET request headers and the target URI, which is
		"GET / HTTP/1.1\r\nHost: example.com\r\n\r\n"
		The send() function is used to send the GET request to the server. It takes four arguments:
		the file descriptor of the socket (sock), a pointer to the request buffer (request.c_str()),
		the size of the request buffer (request.length()), and a set of flags (0 in this case).
	*/

		std::string request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
		send(sock, request.c_str(), request.length(), 0);


	//! Receive the response
	/*
		The recv() function is used to receive the response from the server. It takes four arguments:
		the file descriptor of the socket (sock), a pointer to the response buffer (response), the size
		of the response buffer (sizeof(response) - 1), and a set of flags (0 in this case).
	*/

		char response[4096];
		int response_len = recv(sock, response, sizeof(response) - 1, 0);
		response[response_len] = '\0';


	//! Print the response
	/*
		The code then prints the response by passing the response buffer to cout
	*/

		std::cout << response << std::endl;


	//! Close the socket
	/*
		Finally, the code closes the socket using the close() function to release
		any resources associated with the socket.
	*/
		close(sock);

	return 0;
}

