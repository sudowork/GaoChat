#include "common.h"
#include "gaosocket.h"
#include "server.h"
//#include <pthread.h>	// Used for multi-threading

using namespace std;

// class Server
//

	Server::Server() {
		// Set default port
		this->_port = S_PORT_LST;
	}

	Server::Server(unsigned short port) {
		this->_port = port;
	}

	Server::~Server() {
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
		// TEMPORARY: ITERATIVE ACCEPT
		for (;;) {
			// Declare buffer and msgsize
			char buffer[RECV_BUFFER_SIZE];
			int msgSize;
			memset(&buffer, 0, RECV_BUFFER_SIZE);

			// Create new socket to receive from
			// Note: automatic cleanup upon exiting loop
			TCPSocket* t = (this->_sock)->accept();
			print("Message From Client `" + t->getRemoteAddr() + "`:");

			// Receive and print
			string msg = "";
			while ((msgSize = t->recv(buffer,RECV_BUFFER_SIZE)) > 0) {
				msg += c2substr(buffer);
				memset(buffer, 0, RECV_BUFFER_SIZE);
			}

			/* TODO
			 * Process messages
			 * Handle messages over 32 characters better
			 */
			print(msg);
		}
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

	int setNonBlocking() {
	}

//
// END class Server

int main(int argc, char* argv[]) {
	Server *s = new Server();
	s->start();
	return 0;
}
