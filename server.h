#ifndef SERVER_H
#define SERVER_H

class Server {
	public:
		// Constructors and destructor
		Server();
		Server(unsigned short port);
		~Server();

		void setPort();
		unsigned short getPort();

		bool start();						// Start server on default port
		bool start(unsigned short port);	// Start server with custom port
		bool isRunning();					// Returns boolean based on run status
		void stop();						// Stop server
	private:
		unsigned short _port;
		TCPSocket* _sock;
};

#endif
