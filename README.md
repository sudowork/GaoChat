GaoChat
---------------

@product	GaoChat

@version	0.2

@author		Kevin Gao

### What is Gao Chat?
GaoChat is a C++ based chat server and client. This is not a serious project; instead, it is just an academic assignment used to explore the use of sockets in networking. This project is being made for ECE 156 at Duke University, taught by Romit Choudhury in the Fall 2011 semester.  

### Downloading
The source for the project can be found at the following git repo:
    http://github.com/sudowork/GaoChat

### Installing
To install, simply run the make all. No configuration is necessary since only \*nix platforms are supported at the moment

### Commands
* `/quit` Depending on if a private chat it initiated, this will either quit your private chat or exit the chat client completely
* `/msg <NICKNAME>` Initiates private messages to a peer. Messages will be sent only to the specified peer until quitting
* `/bootstrap <IP:PORT> <NICKNAME>` informs server of the client's IP address, listening port, and nickname; also, a list of peers is requested from the server
* `/getpeers` Requests an updated peer list

### Extra features
* Multiple clients (99) can be run on one host (iterative port)
* The default chat is a group chatroom
* Each client features a unique nickname (if taken, then randomly assigned)
* Private messaging (one on one chat)
	* `/msg <nick>` command
	* Double clicking username in online list
	* Multiple one on one chats
* GUI for client
	* Tabbed messaging
	* Group chat room with list of online peers
	* Push notification of messages
	* Colorized usernames

### Future features
* Improve validation
* Multiple group chats
* More commands
* Ability to choose between UDP or TCP
* NAT traversal via UDP holepunching
* Windows compatibility
* Allow server to be given direct user input to control server
* User statuses
* User timeout

### Contact
You can reach me at `kevin<dot>gao<at>duke<dot>edu`


Screenshots and Demonstration
---------------

Please locate screenshots in the `./docs/` directory. A video demonstration of the server and client is located at the following YouTube link:

[YouTube Link to demonstration](http://www.youtube.com/watch?v=y8ixN6Gtrf4)

Documentation
---------------

## GaoSocket.cpp
This file provides a wrapper for standard C sockets. As of this version, only TCP sockets are supported. The file features a useful abstraction of the Berkeley socket API by combining commonly used commands, and eliminating the need to verbosely describe all the arguments required for the API.
A central class exists, called **Socket**. This class contains the fundamental functions necessary to open a communicating socket (both to send or receive). This is class, however, is not meant to be used directly. There is a publicly accessible class specifically for TCP sockets, called **TCPSocket**. Construction of a **TCPSocket** object instantiates a **Socket** object and then either performs the connection to the remote IP address and port through the *connect()* function, or can be used to *listen()* and *bind()* to a port.

## Common.cpp
This file just provides a basic library that contains several constants and utility functions.


## Server.cpp
Below is a list of the member functions for the Server class, as well as a description of each

### Server();
Constructs a new Server object with a default port of constant value S\_PORT as defined in common.h

### Server(unsigned short port);
Constructs a new Server object with a specified port

### ~Server();
Destructs the Server object. More Importantly, closes the listening socket, freeing the port and file descriptor.

### bool start();
Starts the server by instantiating a new TCPSocket for listening on whatever the server's port is set to. In addition, this function begins an infinite iterative accept loop that waits on input. Within this loop, the function also checks if any commands were passed to the server and performs the appropriate action; if not, then it just echoes the received data to STDOUT. Returns true if successfully run; false otherwise. If the server receives a command, such as /bootstrap, from the client, then it will respond accordingly. Responses will be prefaced by the original command.

### bool isRunning();
Gives the status of the server.

### void stop();
Stops the server by destroying the socket

### int sendMsg(string input, string remoteAddr, unsigned short remotePort);
Sends a message, presumeably, to a client after receiving a command. It creates a new TCPSocket and sends the message to the client. Returns 0 if successfully sent; -1 otherwise.


## Client.cpp
This is the implementation used to create a client that can communicate with the server as specified above.

### ~Client();
### std::string server() const;
Getter for the target server address. Returns IPv4 formatted string containing server address.

### bool server(std::string ip);
Checks if printable IPv4 address is valid and then sets the server address to it. Returns true if successfully set; otherwise, false.

### std::string nick() const;
Getter for client's nick name. Returns nickname as as string.

### void nick(std::string nick);
Setter for client's nick name.

### int listenPID() const;
Getter for child process's PID

### void listenPID(int pid);
Setter for child process's PID

### bool isValidIP(std::string ip);
Checks if printable IPv4 address string is a valid IPv4 address using `inet_pton()`. Returns true if valid; returns false if invalid.

### bool bootstrap();
Attempts to send a bootstrap command to the specified server. If command is sent successfully, then returns true; otherwise, returns false.

### void listen();
A function meant to run in a forked process. This simply listens for data and echoes it out. Note that listening ports are iterated from 9002 to 9099 to allow for multiple clients on one host.

### void serverResp();
To be called after a command has been sent to the server that expects data to be returned. Handles data accordingly.

### void userInput();
Main loop of the program. Awaits user input, checks if it is a command, and takes appropriate action depending on if it is or isn't.


## Interfacing the Server and Client
The server and client currently send data only as strings. If a message does not follow a valid command, then it is assumed to be plaintext and sent to the appropriate channel or peer by the server. If it is a command, then the server will process it and send a reply if necessary. Currently, only the `/bootstrap` command receives feedback from the server. It is in the format of:

    /bootstrap IP:PORT,NICKNAME;IP2:PORT2:NICKNAME2;

In general, the steps needed to write a client capable of interfacing with the server is outlined below:

* Run a loop waiting for user input
* Establish an asynchronous send and receive through a given method
	* Fork a process
	* Multithread your process
	* Use event-based IO
	* Utilize some form of IOCTL in combination with threading
	* Use the **select()** function
* Obtain the server IP and desired nickname from the user
* Open, listen, and bind to an open port.
* Send the bootstrap command as `/bootstrap <LISTENINGPORT> <NICKNAME>`
* Receive the list of peers returned by the server and parse it into a local list or map
* Continue running your user input loop
	* User may choose to message a single user using some command (this is dependent on the client)
	* User may choose to message the entire channel; in that case simply send a plaintext message to the srver
* In order to quit, the command `/quit` must be given to quit cleanly and remove oneself from the server's list of clients
