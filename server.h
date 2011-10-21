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
		// Client maps to maintain unique addresses and nicknames
		std::map<std::string,std::string> _clients_IP_N;	// Map. <IP:PORT,NICK>
		std::map<std::string,std::string> _clients_N_IP;	// Map. <NICK,IP:PORT>

		int sendMsg(std::string input, std::string remoteAddr, unsigned short remotePort);
		int blastMsg(std::string msg);
		bool removeClientByNick(std::string nick);
		bool removeClientByIPP(std::string ipp);
		void addClient(std::string ipp, std::string nick);
		int sendClientsList();
};

#endif
