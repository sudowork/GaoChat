#include "common.h"
#include "gaosocket.h"
#include "client.h"

// Prompts for server IP and then sets; loop until valid IP is given
void promptArgs(Client* client) {
	// Ask for server IP
	std::string ip;
	bool first = true;

	do
	{
		if (!first) print("Invalid IP. Try Again. Format should be 255.255.255.255");
		ip = prompt("Server: ");
		first = false;
	} while (!client->server(ip));

	print("Server set to `" + client->server() + "`");
	
	// Prompt for nickname
	std::string nick;
	bool valid;

	do
	{
		valid = true;
		nick = prompt("Nickname: ");
		if (nick.find(CMD_DELIM) < nick.npos) {
			valid = false;
			print("Nickname may not contain spaces");
		}
		if (nick.length() > 10) {
			valid = false;
			print("Nickname may not be more than 10 characters");
		}
	} while (!valid);
	client->nick(nick);

	print("Nickname set to `" + client->nick() + "`");
	
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
		promptArgs(c);
	}

	// Perform bootstrap to get peers
	c->bootstrap();

	// Fork a process for receiving messages and taking user input
	int pid = fork();
	if (pid < 0) error("Error during fork");
	if (pid == 0) {
		c->listenPID(getpid());
		c->listen();
	}

	// Start accepting user input
	c->userInput();

	return 0;
}
