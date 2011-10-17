#ifndef CLIENT_H
#define CLIENT_H

#include <vector>

class Client {
	public:
		// Constructor and Destructor
		Client();
		~Client();

		std::string server() const; 		// Getter for server
		bool server(std::string ip);   	// Setter for server
		std::string nick() const; 		// Getter for nick
		void nick(std::string nick);   	// Setter for nick
		int listenPID() const;
		void listenPID(int pid);

		// Member functions
		bool isValidIP(std::string ip);			// Checks if IP address is valid
		bool bootstrap();					// Requests peer list from server
		void listen();							// Listens for peer
		void serverResp();						// Listens once for server response
		void userInput();

	private:
		unsigned short _port;					// Client listening port
		TCPSocket* _sock;						// Socket that client listens with
		std::string _server;					// IP address of server used for bootstrapping
		bool _p2p;								// Flag for when P2P chat is initiated
		std::string _peerip;					// Peer IP
		unsigned short _peerport;				// Peer Port
		std::string _peernick;					// Peer nickname
		std::string _nick;					// Nickname
		std::map<std::string,std::string>  _peers;		// Map of peers, <NICK,IP:PORT>
		int _listenPID;							// Process ID of listening child

		int sendMsg(std::string input, std::string remoteAddr, unsigned short remotePort);	// Sends message to server or peer
		void printAvailPeers();					// Prints list of available peers
};

#endif
