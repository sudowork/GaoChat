#include "common.h"		// Shared constants and functions
#include "gaosocket.h"	// Includes socket dependencies 
#include "client.h"

using namespace std;

// class Client
//

// Constructor
Client::Client() {
	_server = "127.0.0.1";	// Set default value of server
	_serverPort = S_PORT;	// Set to default port
	_listenPID = -1;
	initListen();
}

Client::Client(string server, unsigned short port) {
	_server = server;
	_serverPort = port;
	_listenPID = -1;
	initListen();
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
int Client::sockFd() const { return _sock->sockFd(); }
map<string,string> Client::getPeers() { return _peers; }

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
		return -1;
		/* TODO
		 * ADD ERROR CHECKING
		 */
	}

	// Close socket
	delete sockSend;

	return 0;
}

void Client::initListen() {
	// Create listening port
	this->_sock = new TCPSocket();
	//Set listening socket to nonblocking
	_sock->setNonBlocking();
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

int Client::sendServerMsg(string input) {
	return sendChatMsg(input,_server,_serverPort);
}

int Client::sendPeerMsg(string input, string nick) {
	IPPort ipp = parseIPPstr(_peers.find(nick)->second);
	return sendChatMsg(input,ipp.ip,ipp.port);
}

int Client::sendChatMsg(string input, string ip, unsigned short port) {
	// Check for command
	if (isCmd(input)) {
		Command c = str2cmd(input);

		// Stop taking input if quit command is given
		if (c.isValid && c.cmd.compare(QUIT) == 0) {
			// Inform remote side that quitting
			return sendMsg(CMD_ESCAPE + QUIT + CMD_DELIM + _nick,ip,port);
		}

		// Handle message command
		if (c.isValid && c.cmd.compare(MSG) == 0) {
			// Get nickname, lookup in peer map, and establish connection
			string n = c.args[0];
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
				print("Messaging with " + n + " enabled");
				print("To stop messaging this person, use `/quit`");
			} else {
				print("Invalid peer name");
			}
		}

		// Handle getpeers
		if (c.isValid && c.cmd.compare(GETPEERS) == 0) {
			char myport[5];
			sprintf(myport,"%u",_port);
			return sendMsg(CMD_ESCAPE + GETPEERS + CMD_DELIM + myport,_server,_serverPort);
		}

		// Handle change nickname
		if (c.isValid && c.cmd.compare(NICK) == 0) {
			char myport[5];
			sprintf(myport,"%u",_port);
			return sendMsg(CMD_ESCAPE + BOOTSTRAP + CMD_DELIM + myport + CMD_DELIM + c.args[0],_server,_serverPort);
		}
	}
	return sendMsg(input,ip,port);
}

bool Client::isConnected() {
	try {
		sendMsg(CMD_ESCAPE + ISCON,_server,_serverPort);
	} catch (exception &e) {
		return false;
	}
	return true;
}

// Connects to server and requests list of peers from server
bool Client::bootstrap() {
	print("Bootstrapping with `" + this->server() + "`...");

	// Bootstrap command `/bootstrap <clientport> <nickname>`
	char port[5];
	sprintf(port,"%u",this->_port);
	string bsCmd = CMD_ESCAPE + BOOTSTRAP + CMD_DELIM + port + CMD_DELIM + _nick;

	// Send bootstrap command
	sendMsg(bsCmd,_server,_serverPort);

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
			unsigned short port = (_p2p) ? _peerport : _serverPort;

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
						if (_listenPID > 0) {
							kill(_listenPID,1);
						}
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
							print("Messaging with " + n + " enabled");
							print("To stop messaging this person, use `/quit`");
						} else {
							print("Invalid peer name");
						}
					} else {
						print("No argument supplied, the command is `/msg <NICK>`");
					}
				}

				// Handle getpeers
				if (str2cmd(input).cmd.compare(GETPEERS) == 0) {
					char myport[5];
					sprintf(myport,"%u",_port);
					sendMsg(CMD_ESCAPE + GETPEERS + CMD_DELIM + myport,_server,_serverPort);
				}
			} else {
				// Send message
				// If p2p mode, prepend @nick
				if (_p2p) input = "\n@" + _nick + ">" + input;
				else input = _port + ":\n" + _nick + ">" + input;
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
		print(readSocket());
	}
}

string Client::readSocket() {
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
	}

	return msg;
}

void Client::addPeer(string ip, string nick) {
	_peers.insert(pair<string,string>(nick,ip));
}

void Client::clearPeers() {
	_peers.clear();
}

/*void Client::serverResp() {
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
		if (isCmd(msg) && (str2cmd(msg)).isValid) {
			// Split cmd
			string cmd = msg.substr(1,msg.find(CMD_DELIM)-1);

			// Handle bootstrap return
			if (cmd.compare(RETPEERS) == 0) {
				// Clear old list of peers
				_peers.erase(_peers.begin(),_peers.end());
				// string is formatted as IP:PORT,NICK;
				// explode string and insert into map of peers
				vector<string> v = explode(msg.substr(msg.find(CMD_DELIM)+1),';');
				// Enter each peer into map
				for (int i = 0; i < v.size(); i++) {
					vector<string> p = explode(v[i],',');
					_peers.insert(pair<string,string>(p[1],p[0]));
				}
				printAvailPeers();
			}
		}
	} else {
	}
}*/

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