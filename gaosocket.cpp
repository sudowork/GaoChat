#include "common.h"
#include "gaosocket.h"
// class Socket
//

	Socket::Socket(int type, int protocol = 0) {
		#ifdef WIN32
			// Take care of WinSock creation
			//////////////////////
		#else
			// Create new Unix socket
			// and set socket descriptor
			if ((sockDesc = socket(AF_INET, type, protocol)) == -1) {
				// socket creation unsuccessful
				print("error creating socket");
				//////////////////////
			}
		#endif
	}

	// Clean up socket (destructor)
	Socket::~Socket() {
		#ifdef WIN32
			// Perform WinSock cleanup
			//////////////////////
		#else
			close(sockDesc);
		#endif
	}

//
// END class Socket

// class TCPSocket
//
	
	TCPSocket::TCPSocket() : Socket(SOCK_STREAM) {};
	TCPSocket::TCPSocket(const string &remoteAddr,
		unsigned short remotePort) : Socket(SOCK_STREAM) {

	}
