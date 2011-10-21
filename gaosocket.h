#ifndef GAOSOCKET_H
#define GAOSOCKET_H

// Socket dependencies
// Check if on Windows system -> use WinSock
#ifdef WIN32
	#include <winsock.h>
	typedef int socklen_t;
	typedef char raw_type;
#else
	// Otherwise assume *nix platform
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	typedef void raw_type;
#endif

// Wrapper class <sys/socket.h>
// Communicating socket
class Socket {
	public:
		// Destructor
		~Socket();

		// Getters and setters
		std::string getLocalAddr();
		bool setLocalAddr(const std::string &addr);
		unsigned short getLocalPort();
		bool setLocalPort(unsigned short port);
		bool setLocal(const std::string &addr, unsigned short port);
		int sockFd();

		std::string getRemoteAddr();
		bool setRemoteAddr(const std::string &addr);
		unsigned short getRemotePort();
		bool setRemotePort(unsigned short port);
		bool setRemote(const std::string &addr, unsigned short port);

		// Member functions
		int connect(const std::string &remoteAddr, unsigned short remotePort);
		int send(const void *buffer, int bufferLen);
		int recv(void *buffer, int bufferLen);			// Only to be used on a socket returned from accept()

		static void cleanUp();

	protected:
		int _sockDesc;					// Socket descriptor
		Socket(int type, int protocol);	// Creates a socket of specified type (SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET)
		Socket(int sockDesc);			// Creates a new socket wrapper for an open socket - used by accept() for future recv()
};


// Wrapper for TCP-based socket (type SOCK_STREAM)
class TCPSocket : public Socket {
	public:
		// Constructors
		TCPSocket(); // Default constructor
		TCPSocket(const std::string &remoteAddr, unsigned short remotePort);		// Create a TCP Socket with remote address and port
		TCPSocket(int connDesc);	// Creates a new socket from a new connection (based on file descriptor)

		// Server-side functions
		int bindlisten(unsigned short port, int queueLen=5);
		TCPSocket* accept();

		int setNonBlocking();		// Sets the socket to be nonblocking
		int setBlocking();		// Sets the socket to be nonblocking
};


// Wrapper for UDP-based socket (type SOCK_DGRAM)
class UDPSocket : public Socket {
};

#endif
