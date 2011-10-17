#include "common.h"		// Shared constants and functions
#include "gaosocket.h"	// Includes socket dependencies 
#include "client.h"

using namespace std;

// class Client
//

	// Constructor
	Client::Client() {
		_server = "127.0.0.1";	// Set default value of server
		// Create listening port
		this->_sock = new TCPSocket();
		// Set listening socket to nonblocking
		//_sock->setNonBlocking();
		// Try to listen on ports ranging from C_PORT to C_PORT_END
		for (unsigned short i=C_PORT; i <= C_PORT_END; i++) {
			if ((this->_sock)->bindlisten(i) < 0) {
				if (i == C_PORT_END) {
					error("Could not bind to port");
				}
				continue;
			} else {
				this->_port = i;
				break;		// Stop looping
			}
		}
	}

	Client::~Client() {
	}

	// Getters and Setters
	string Client::server() const { return _server; }				// Getter for server
	bool Client::server(string ip) {								// Setter for server
		if (isValidIP(ip)) { _server = ip; return true; }
		else { return false; }
	}
	string Client::nick() const { return _nick; }				// Getter for nick
	void Client::nick(string nick) {								// Setter for nick
		this->_nick = nick;
	}
	int Client::listenPID() const { return _listenPID; }
	void Client::listenPID(int pid) { _listenPID = pid; }

	// Member functions
	bool Client::isValidIP(string ip) {
		struct sockaddr_in sa;
		return inet_pton(AF_INET,ip.c_str(),&(sa.sin_addr)) == 1;
	}

	int Client::sendMsg(string input, string remoteAddr, unsigned short remotePort) {
		// Send stream 
		// Create socket connection with server
		TCPSocket* sockSend = new TCPSocket(remoteAddr,remotePort);

		char *msg = (char *) malloc(input.length() * sizeof(char));
		msg = (char *)input.c_str();

		// Send stream
		if (sockSend->send(msg,strlen(msg)) == -1) {
			/* TODO
			 * ADD ERROR CHECKING
			 */
		}

		// Close socket
		delete sockSend;
	}
	
	// Connects to server and requests list of peers from server
	bool Client::bootstrap() {
		print("Bootstrapping with `" + this->server() + "`...");

		// Bootstrap command `/bootstrap <clientport> <nickname>`
		char port[5];
		sprintf(port,"%u",this->_port);
		string bsCmd = CMD_ESCAPE + BOOTSTRAP + CMD_DELIM + port + CMD_DELIM + _nick;

		// Send bootstrap command
		sendMsg(bsCmd,_server,S_PORT);
		serverResp();

		return true;
	}

void Client::userInput() {
	// Loop for input
	try {
		// Wait for input
		for (;;) {
			string input;
			
			// Set send address and port based on if in conversation
			string ip = (_p2p) ? _peerip : _server;
			unsigned short port = (_p2p) ? _peerport : S_PORT;

			cout << _nick + ">";
			getline(cin,input);
			
			// Check for command
			if (isCmd(input)) {
				// Stop taking input if quit command is given
				if ((str2cmd(input).cmd).compare(QUIT) == 0) {
					// Inform remote side that quitting
					sendMsg(CMD_ESCAPE + QUIT + CMD_DELIM + _nick,ip,port);
					// Check if in P2P conversation
					if (_p2p) {
						// Just leave conversation
						_p2p = false;
						print("Leaving conversation");
					} else {
						// Quit program
						// Kill child PID
						kill(_listenPID,1);
						break;
					}
				}

				// Handle message command
				if (str2cmd(input).cmd.compare(MSG) == 0) {
					// Check for argument
					if (input.find(CMD_DELIM) < input.npos) {
						// Get nickname, lookup in peer map, and establish connection
						string n = input.substr(input.find(CMD_DELIM)+1);
						if (_peers.find(n) != _peers.end()) {
							// Peer in peer map
							// Establish send with peer
							print("Establishing conversation with " + n);
							// Set P2P flag and set peer IP and port
							_p2p = true;
							IPPort ipp = parseIPPstr(_peers[n]);
							_peerip = ipp.ip;
							_peerport = ipp.port;
							_peernick = n;
							print("Connection established");
						} else {
							print("Invalid peer name");
						}
					} else {
						print("No argument supplied, the command is `/msg <NICK>`");
					}
				}
			} else {
				// Send message
				// If p2p mode, prepend nick
				if (_p2p) input = "\n" + _nick + ">" + input;
				sendMsg(input,ip,port);
			}

		}
	} catch (exception &e) {
		cerr << e.what() << endl;
		exit(1);
	}
}

void Client::listen() {
	for (;;) {
		// Declare buffer and msgsize
		char buffer[RECV_BUFFER_SIZE];
		int msgSize;
		memset(&buffer, 0, RECV_BUFFER_SIZE);

		// Create new socket to receive from
		// Note: automatic cleanup upon exiting loop
		TCPSocket* t = (this->_sock)->accept();

		// Receive and print
		string msg = "";
		while ((msgSize = t->recv(buffer,RECV_BUFFER_SIZE)) > 0) {
			msg += c2substr(buffer);
			memset(buffer, 0, RECV_BUFFER_SIZE);
		}

		if (isCmd(msg) && (str2cmd(msg)).isValid) {
		} else {
			print(msg);
		}
	}
}

void Client::serverResp() {
	// Declare buffer and msgsize
	char buffer[RECV_BUFFER_SIZE];
	int msgSize;
	memset(&buffer, 0, RECV_BUFFER_SIZE);

	// Create new socket to receive from
	// Note: automatic cleanup upon exiting loop
	TCPSocket* t = (this->_sock)->accept();

	// Receive and print
	string msg = "";
	while ((msgSize = t->recv(buffer,RECV_BUFFER_SIZE)) > 0) {
		msg += c2substr(buffer);
		memset(buffer, 0, RECV_BUFFER_SIZE);
	}
	// Check if response from server
	if (t->getRemoteAddr().compare(_server) == 0) {
		// If command, perform action
		if (isCmd(msg)) {
			// Split cmd
			string cmd = msg.substr(1,msg.find(CMD_DELIM)-1);

			// Handle bootstrap return
			if (cmd.compare(BOOTSTRAP) == 0) {
				// string is formatted as IP:PORT,NICK;
				// explode string and insert into map of peers
				vector<string> v = explode(msg.substr(msg.find(CMD_DELIM)+1),';');
				// Enter each peer into map
				for (int i = 0; i < v.size(); i++) {
					vector<string> p = explode(v[i],',');
					_peers.insert(pair<string,string>(p[0],p[1]));
				}
				printAvailPeers();
			}
		}
	} else {
	}
}

void Client::printAvailPeers() {
	print();
	print("Available Peers");
	print("To message a peer use the command `/msg <NICKNAME>`");
	print("----------");
	map<string,string>::iterator it;
	for (it = _peers.begin(); it != _peers.end(); it++) {
		print((*it).first);
	}
	print();
}

//
// END class Client

// Prompts for server IP and then sets; loop until valid IP is given
void promptArgs(Client* client) {
	// Ask for server IP
	string ip;
	bool first = true;

	do
	{
		if (!first) print("Invalid IP. Try Again. Format should be 255.255.255.255");
		ip = prompt("Server: ");
		first = false;
	} while (!client->server(ip));

	print("Server set to `" + client->server() + "`");
	
	// Prompt for nickname
	string nick;
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
