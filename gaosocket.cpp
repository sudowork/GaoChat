#include "common.h"
#include "gaosocket.h"
#include <fcntl.h>		// Used for retrieving descriptors

using namespace std;

// class Socket
//

	Socket::Socket(int type, int protocol) {
		#ifdef WIN32
			// Take care of WinSock creation
			//////////////////////
		#else
			// Create new Unix socket
			// and set socket descriptor
			if ((_sockDesc = ::socket(AF_INET, type, protocol)) == -1) {
				// socket creation unsuccessful
				error("Error creating socket");
			}
		#endif
	}

	Socket::Socket(int sockDesc) {
		this->_sockDesc = sockDesc;
	}

	string Socket::getLocalAddr() {
		sockaddr_in addr;
		unsigned int addr_len = sizeof(addr);
		if (getsockname(this->_sockDesc, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0) {
			error("Could not get address of peer");
		}
		return inet_ntoa(addr.sin_addr);
	}

	short unsigned Socket::getLocalPort() {
		sockaddr_in addr;
		unsigned int addr_len = sizeof(addr);
		if (getsockname(this->_sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
			error("Could not get port of peer");
		}
		return ntohs(addr.sin_port);
	}

	string Socket::getRemoteAddr() {
		sockaddr_in addr;
		unsigned int addr_len = sizeof(addr);
		if (getpeername(this->_sockDesc, (sockaddr *) &addr,(socklen_t *) &addr_len) < 0) {
			error("Could not get address of peer");
		}
		return inet_ntoa(addr.sin_addr);
	}

	short unsigned Socket::getRemotePort() {
		sockaddr_in addr;
		unsigned int addr_len = sizeof(addr);
		if (getpeername(this->_sockDesc, (sockaddr *) &addr, (socklen_t *) &addr_len) < 0) {
			error("Could not get port of peer");
		}
		return ntohs(addr.sin_port);
	}

	int Socket::sockFd() {
		return _sockDesc;
	}

	int Socket::connect(const string &remoteAddr, unsigned short remotePort) {
		sockaddr_in serverAddr;
		memset(&serverAddr,0,sizeof(serverAddr));	// Overwrite serverAddr struct with 0s
		serverAddr.sin_family = AF_INET;			// IPv4 family
		
		inet_pton(AF_INET,remoteAddr.c_str(),&(serverAddr.sin_addr));	// Convert IP address to network format
		serverAddr.sin_port = htons(remotePort);	// host to network order for port

		int connection;
		if ((connection = ::connect(_sockDesc,&(sockaddr&)serverAddr,sizeof(serverAddr))) < 0) {
			error("Error connecting");
			return -1;
		} else {
			//print("Successfully connected");
			return 0;
		}
	}

	int Socket::send(const void *buffer, int bufferLen) {
		if (::send(_sockDesc,(raw_type *)buffer,bufferLen,0) < 0) {
			error("Error sending");
			return -1;
		} else {
			//print("Send successful");
			return 0;
		}
	}

	int Socket::recv(void *buffer, int bufferLen) {
		int mSize;
		if ((mSize = ::recv(_sockDesc,(raw_type *)buffer,bufferLen,0)) < 0) {
			error("Error receiving");
			return -1;
		} else {
			return mSize;
		}
	}


	// Clean up socket (destructor)
	Socket::~Socket() {
		#ifdef WIN32
			// Perform WinSock cleanup
			//////////////////////
		#else
			// Shutdown and close socket
			::shutdown(_sockDesc,2);
			::close(_sockDesc);
		#endif
	}

//
// END class Socket

///////////////////////////////////////////////////////////

// class TCPSocket
//
	
	TCPSocket::TCPSocket() : Socket(SOCK_STREAM, IPPROTO_TCP) {};
	TCPSocket::TCPSocket(const string &remoteAddr,
		unsigned short remotePort) : Socket(SOCK_STREAM, IPPROTO_TCP) {
		connect(remoteAddr,remotePort);	// Establish connection after creating socket descriptor
	}
	TCPSocket::TCPSocket(int connDesc) : Socket(connDesc) {};

	int TCPSocket::bindlisten(unsigned short port, int queueLen) {
		sockaddr_in addr;
		// Fill in address structure
		memset(&addr,0,sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port);

		// Attempt to bind to socket
		if (::bind(_sockDesc,&((sockaddr &)addr),sizeof(addr)) < 0) {
			error("Error binding to socket on port");
			return -1;
		}

		// Attempt to start listening
		if (::listen(_sockDesc,queueLen) < 0) {
			error("Error listening to port");
			return -1;
		}

		print("Listening on port");
		return 0;
	}

	TCPSocket* TCPSocket::accept() {
		// Open a new file descriptor by accept()
		int connDesc = ::accept(_sockDesc,(struct sockaddr *)NULL,0);

		// Check if accept() worked
		if (connDesc < 0) {
			error("Error accepting connection");
		}
		
		return new TCPSocket(connDesc);
	}

	int	TCPSocket::setNonBlocking() {
		int flags;
		// Check if POSIX compatible
		#ifdef O_NONBLOCK
			if ((flags = fcntl(this->_sockDesc,F_GETFL,0)) == -1)
				flags = 0;
			return fcntl(this->_sockDesc, F_SETFL, flags | O_NONBLOCK);
		#else
			flags = 1;
			return ioctl(this->_sockDosc, FIOBIO, &flags);
		#endif
	}

	

//
// END class TCPSocket
