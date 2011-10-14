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

		// Member functions
		bool isValidIP(std::string ip);			// Checks if IP address is valid
		bool bootstrap();					// Requests peer list from server
		std::vector<std::string> bufferString(std::string str, int bufferLen=RECV_BUFFER_SIZE);	// Takes std::string of unknown size and buffers into std::strings of appropriate length

	private:
		std::string _server;					// IP address of server used for bootstrapping
		std::string _nick;					// Nickname
		struct in_addr*  _peers;		// Dynamic array of peers in the form of IPv4 addresses, to be filled upon retrieval
};

#endif
