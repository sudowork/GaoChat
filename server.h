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
		bool isRunning();					// Returns boolean based on run status
		void stop();						// Stop server
	private:
		unsigned short _port;				// Listening port
		TCPSocket* _sock;					// Listening socket
		std::map<std::string,std::string> _clients;	// Map. <IP:PORT,NICK>

		int sendMsg(std::string input, std::string remoteAddr, unsigned short remotePort);
};

#endif
