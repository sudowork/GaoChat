#include "common.h"		// Shared constants and functions
#include "gaosocket.h"	// Includes socket dependencies 
#include "client.h"

using namespace std;

// class Client
//

	// Constructor
	Client::Client() {
		_server = "127.0.0.1";	// Set default value of server
	}

	Client::~Client() {
	}

	// Getters and Setters
	string Client::server() const { return _server; }				// Getter for server
	bool Client::server(string ip) {								// Setter for server
		if (isValidIP(ip)) { _server = ip; return true; }
		else { return false; }
	}

	// Member functions
	bool Client::isValidIP(string ip) {
		struct sockaddr_in sa;
		return inet_pton(AF_INET,ip.c_str(),&(sa.sin_addr)) == 1;
	}
	
	// Connects to server and requests list of peers from server
	bool Client::bootstrap() {
		print("Bootstrapping with `" + this->server() + "`...");

		/* TODO
		 * perform actual bootstrap function
		 * */

		/* TODO
		 * separate into separate function
		 */
		try {
			// Wait for input
			for (;;) {
				string input;

				cout << ">";
				getline(cin,input);
				
				// Check for command
				if (isCmd(input)) {
					// Stop taking input if quit command is given
					if ((str2cmd(input).cmd).compare(QUIT) == 0) break;
				}

				// Send stream 
				// Create socket connection with server
				TCPSocket* sockSend = new TCPSocket(_server,S_PORT_LST);

				char *msg = (char *) malloc(input.length() * sizeof(char));
				input.copy(msg,input.length());

				// Send stream
				if (sockSend->send(msg,strlen(msg)) == -1) {
					/* TODO
					 * ADD ERROR CHECKING
					 */
				}

				// Close socket
				delete sockSend;
			}
		} catch (exception &e) {
			cerr << e.what() << endl;
			exit(1);
		}
		return true;
	}

//
// END class Client

// Prompts for server IP and then sets; loop until valid IP is given
void promptServer(Client* client) {
	string ip;
	bool first = true;

	do
	{
		if (!first) print("Invalid IP. Try Again. Format should be 255.255.255.255");
		ip = prompt("Server: ");
		first = false;
	} while (!client->server(ip));

	print("Server set to `" + client->server() + "`");
}

int main(int argc, char *argv[]) {
	Client *c = new Client();	// Construct new client object
	// Handle arguments/options
	// Set server address
	if (argc > 1) {
		int i = 1;
		while (i < argc) {
			// Process options
			////////////////////////////////////////
		}
		// Set server info
		////////////////////////////////////////
	} else {
		// If no arguments given, ask for server and set in client
		promptServer(c);
	}

	// Perform bootstrap to get peers
	c->bootstrap();

	return 0;
}
