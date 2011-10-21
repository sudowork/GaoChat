#ifndef COMMON_H
#define COMMON_H

// Common libraries
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <csignal>
#include <exception>
#include <cerrno>

// Constants
#define S_PORT 9001		// Server recv port
#define	C_PORT 9002		// Client listen port
#define C_PORT_END 9099	// Client listen port end range
#define RECV_BUFFER_SIZE 32	// Buffer size for receiving data

// Commands for chat
#define CMD_ESCAPE '/'			// Escape character for command
#define CMD_DELIM ' '			// Delimiter for command

static const std::string BOOTSTRAP	= "bootstrap";		// Requests peer list from server
static const std::string QUIT		= "quit";			// Quits client
static const std::string NICK		= "nick";			// Sets client
static const std::string MSG		= "msg";			// Messages peer
static const std::string GETPEERS	= "getpeers";		// Gets updated list of peers
static const std::string RETPEERS	= "peers";			// Gets updated list of peers
static const std::string ISCON		= "isconnected";	// Pings server to see if connected

struct Command {
	bool isValid;			// checks if valid command
	std::string cmd;				// Based on predefined preprocessor constants
	std::vector<std::string> args;		// Argument for command
};

struct IPPort {
	std::string ip;
	unsigned short port;
};

// Common functions
void print();								// Outputs blank line to STDOUT
void print(std::string message);			// Outputs a new line to STDOUT
void error(std::string message);			// Outputs new line to STDERR
std::string prompt(std::string message);		// Prompts with message and returns input from STDIN

Command str2cmd(std::string input);		// Returns which command and argument
std::vector<std::string> explode(std::string str, char delim=CMD_DELIM);	// Splits string by delimiter
bool isCmd(std::string input);				// Simply evaluates whether input starts with the escape for commands
std::string c2substr(char * c, unsigned int start=0, unsigned int n=RECV_BUFFER_SIZE);	// returns substring from char array
std::string map2str(std::map<std::string,std::string> m);
IPPort parseIPPstr(std::string ipp);						// Returns an IPPort struct from IP:PORT string

#endif
