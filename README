GaoChat
---------------

@product	GaoChat
@version	0.1
@author		Kevin Gao

### What is Gao Chat?
GaoChat is a C++ based chat server and client. This is not a serious project; instead, it is just an academic assignment used to explore the use of sockets in networking. This project is being made for ECE 156 at Duke University, taught by Romit Choudhury in the Fall 2011 semester.  

### Downloading
The source for the project can be found at the following git repo:
    http://github.com/sudowork/GaoChat

### Installing
To install, simply run the make all. No configuration is necessary since only *nix platforms are supported at the moment

### Commands
* **/quit** Depending on if a private chat it initiated, this will either quit your private chat or exit the chat client completely
* **/msg <NICKNAME>** Initiates private messages to a peer. Messages will be sent only to the specified peer until quitting
* **/bootstrap <IP:PORT> <NICKNAME>** - informs server of the client's IP address, listening port, and nickname; also, a list of peers is requested from the server
* **/getpeers** Requests an updated peer list

### Extra features
* Multiple clients can be run on one computer
* The default chat is a group chatroom
* Each client features a nickname
* It is possible to start private messaging a user, by using the `/msg <nick>` command

### Future features
* Addition of a GUI
* Multiple group chats
* More commands
* Automatic refreshing of peer list
* Ability to choose between UDP or TCP
* Cross-platform compatability

### Contact
You can reach me at kevin<dot>gao<at>duke<dot>edu

Documentation
---------------

## GaoSocket.cpp
This file provides a wrapper for standard C sockets. As of this version, only TCP sockets are supported. The file features a useful abstraction of the Berkeley socket API by combining commonly used commands, and eliminating the need to verbosely describe all the arguments required for the API.
A central class exists, called **Socket**. This class contains the fundamental functions necessary to open a communicating socket (both to send or receive). This is class, however, is not meant to be used directly. There is a publicly accessible class specifically for TCP sockets, called **TCPSocket**. Construction of a **TCPSocket** object instantiates a **Socket** object and then either performs the connection to the remote IP address and port through the *connect()* function, or can be used to *listen()* and *bind()* to a port.

## Common.cpp
This file just provides a basic library that contains several constants and utility functions.

## Server.cpp
	Server();
	Server(unsigned short port);
	~Server();
	void setPort();
	unsigned short getPort();
	bool start();
	bool start(unsigned short port);
	bool isRunning();
	void stop();
