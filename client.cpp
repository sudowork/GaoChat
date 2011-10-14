#include "client.h"

// class Client
//

	// Constructor
	Client::Client() {
		_server = "127.0.0.1";
		inet_pton(AF_INET,_server.c_str(),&(_serversa));	// Set default value of server to localhost
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
	void Client::bootstrap() {
	}

//
// END class Client

void print(string message) {
	cout << message << endl;
}

// Prompts with message, and return input
string prompt(string message) {
	string answer;
	cout << message;
	cin >> answer;
	return answer;
}

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

	print("Server successfully set to " + client->server());
}

int main(int argc, char *argv[]) {
	Client* c = new Client();	// Construct new client object
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
