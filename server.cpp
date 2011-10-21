#include "common.h"
#include "gaosocket.h"
#include "server.h"
//#include <pthread.h>	// Used for multi-threading

using namespace std;

// class Server
//

	Server::Server() {
		// Set default port
		this->_port = S_PORT;
	}

	Server::Server(unsigned short port) {
		this->_port = port;
	}

	Server::~Server() {
		delete this->_sock;
	}

	bool Server::start() {
		// Check if server is already running
		if (this->isRunning()) {
			error("Error: Server is already running");
			return false;
		}
		
		// Open socket
		this->_sock = new TCPSocket();
		(this->_sock)->bindlisten(this->_port);
		
		// Start accepting connections
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

			// Check if command and handle appropriately
			if (isCmd(msg)) {
				Command cmd = str2cmd(msg);
				// If bootstrap command
				if (cmd.isValid && cmd.cmd.compare(BOOTSTRAP) == 0) {
					// Add connecting client to list of clients
					string ippstr = t->getRemoteAddr() + ":" + cmd.args[0];
					addClient(ippstr,cmd.args[1]);

					// Update clients' peer lists
					sendClientsList();
				}
				// If quit, then remove client from list of clients
				if (cmd.isValid && cmd.cmd.compare(QUIT) == 0 ){
					removeClientByNick(cmd.args[0]);
					sendClientsList();
				}

				// If getpeers, send client list
				if (cmd.isValid && cmd.cmd.compare(GETPEERS) == 0) {
					sendClientsList();
				}
			} else {
				// Blast message to all peers - originator
				/*
				unsigned int p;
				istringstream(msg.substr(0,msg.find(':'))) >> p;
				msg.erase(0,msg.find(':')+1);

				map<string,string>::iterator it;
				for (it = _clients_IP_N.begin(); it != _clients_IP_N.end(); it++) {
					IPPort ipp = parseIPPstr((*it).first);
					if (ipp.ip.compare(t->getRemoteAddr()) != 0 || ipp.port != p) {
						sendMsg(msg,ipp.ip,ipp.port);
					}
				}*/
			}
			cout << "`" + t->getRemoteAddr() + "`:";
			print(msg);
		}
	}

	int Server::blastMsg(string msg) {
		map<string,string>::iterator it;
		for (it = _clients_IP_N.begin(); it != _clients_IP_N.end(); it++) {
			IPPort ipp = parseIPPstr((*it).first);
			if (sendMsg(msg,ipp.ip,ipp.port) < 0)
				return -1;
		}
		return 1;
	}

	bool Server::isRunning() {
		return 0;
	}

	void Server::stop() {
		// Clean up socket
		// Note: shutdown/close of socket is performed
		// 	by the socket's destructor
		delete this->_sock;
	}

	int Server::sendMsg(string input, string remoteAddr, unsigned short remotePort) {
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

	bool Server::removeClientByNick(string nick) {
		// Find client by nickname if exists
		// And ensure that matches IP address
		map<string,string>::iterator it = _clients_N_IP.find(nick);
		if (it != _clients_N_IP.end()
				&& (_clients_IP_N.find(it->second)->second.compare(it->first) == 0)) {
			_clients_N_IP.erase(nick);
			_clients_IP_N.erase(it->second);
			return true;
		}
		return false;
	}

	bool Server::removeClientByIPP(string ipp) {
		// Find client by ip address and port if exists
		map<string,string>::iterator it = _clients_IP_N.find(ipp);
		if (it != _clients_IP_N.end()
				&& (_clients_N_IP.find(it->second)->second.compare(it->first) == 0)) {
			_clients_IP_N.erase(ipp);
			_clients_N_IP.erase(it->second);
			return true;
		}
		return false;
	}

	void Server::addClient(string ipp, string nick) {
		unsigned short itNum = 1;
		string newNick = nick;

		// Check if IP and port already exist
		// If exist, then assume nickname change
		if (_clients_IP_N.find(ipp) != _clients_IP_N.end()) {
			// Remove old nickname
			removeClientByIPP(ipp);
		}

		// Check if nickname taken
		print(map2str(_clients_N_IP));
		if (_clients_N_IP.find(newNick) != _clients_N_IP.end())
			print("taken");
		while (_clients_N_IP.find(newNick) != _clients_N_IP.end()) {
			// If taken, append underscore and iterative number
			print("Nickname `" + nick + "` taken");
			char *temp;
			sprintf(temp,"%u",itNum);
			newNick = nick + "_" + temp;
		}
		
		// Add ip, port, and name to map
		_clients_IP_N.insert(pair<string,string>(ipp,newNick));
		_clients_N_IP.insert(pair<string,string>(newNick,ipp));
	}

	int Server::sendClientsList() {
		// Send updated list back to all clients
		print("Sending client list to all clients");

		string mapStr = CMD_DELIM + map2str(_clients_IP_N);
		string baseMsg = CMD_ESCAPE + RETPEERS + CMD_DELIM;

		map<string,string>::iterator it;
		for (it = _clients_IP_N.begin(); it != _clients_IP_N.end(); it++) {
			IPPort ipp = parseIPPstr((*it).first);
			if (sendMsg(baseMsg + it->second + mapStr,ipp.ip,ipp.port) < 0)
				return -1;
		}
		return 1;
	}

//
// END class Server

int main(int argc, char* argv[]) {
	Server *s = new Server();
	s->start();
	return 0;
}
