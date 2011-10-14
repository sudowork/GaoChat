#include <iostream>
#include <string>
#include <exception>
#include "gaosocket.h"

#define	PORT	"9001";		// Port number for socket communication

using namespace std;

class Client {
	public:
		Client();	// Constructor
		~Client();	// Destructor

		string server() const; 		// Getter for server
		bool server(string);   		// Setter for server

		// Member functions
		bool isValidIP(string);		// Checks if IP address is valid
		void bootstrap();			// Requests peer list from server

	private:
		string _server;					// IP address of server used for bootstrapping
		struct sockaddr_in _serversa;	// Server socket address
		struct in_addr*  _peers;		// Dynamic array of peers in the form of IPv4 addresses, to be filled upon retrieval
};
