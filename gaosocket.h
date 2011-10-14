// Socket dependencies
// Check if on Windows system -> use WinSock
#ifdef WIN32
	#include <winsock.h>
	typedef int socklen_t;
	typedef char raw_type;
// Otherwise assume *nix platform
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	typedef void raw_type;
#endif

using namespace std;

// Wrapper class <sys/socket.h>
// Communicating socket
class Socket {
	public:
		// Constructor and destructor
		~Socket();

		// Getters and setters
		string getLocalAddr();
		bool setLocalAddr(const string &addr);
		unsigned short getLocalPort();
		bool setLocalPort(unsigned short port);
		bool setLocal(const string &addr, unsigned short port);

		string getRemoteAddr();
		bool setRemoteAddr(const string &addr);
		unsigned short getRemotePort();
		bool setRemotePort(unsigned short port);
		bool setRemote(const string &addr, unsigned short port);

		// Member functions
		void connect(const string &remoteAddr, unsigned short remotePort);
		void send(const void *buffer, int bufferLen);
		void recv(void *buffer, int bufferLen);

		static void cleanUp();

	protected:
		int sockDesc;					// Socket descriptor
		Socket(int type, int protocol);	// Creates a socket of specified type (SOCK_STREAM, SOCK_DGRAM, SOCK_SEQPACKET)
};

// Wrapper for TCP-based socket (type SOCK_STREAM)
class TCPSocket : public Socket {
	public:
		TCPSocket(); // Default constructor
		TCPSocket(const string &remoteAddr, unsigned short remotePort);		// Create a TCP Socket with remote address and port
};

// Wrapper for UDP-based socket (type SOCK_DGRAM)
class UDPSocket : public Socket {
};
