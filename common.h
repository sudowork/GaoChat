#ifndef COMMON_H
#define COMMON_H

// Common libraries
#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <cerrno>

// Constants
#define	C_PORT 9001		// Client send port
#define	C_PORT_LST 9002	// Client recv port
#define S_PORT 9003		// Server send port
#define S_PORT_LST 9004	// Server recv port
#define RECV_BUFFER_SIZE 32	// Buffer size for receiving data

// Commands for chat
#define CMD_ESCAPE '/'			// Escape character for command
#define CMD_DELIM ' '			// Delimiter for command

static const std::string BOOTSTRAP	= "bootstrap";	// Requests peer list from server
static const std::string QUIT		= "quit";		// Quits client

struct Command {
	bool isValid;			// checks if valid command
	std::string cmd;				// Based on predefined preprocessor constants
	std::vector<std::string> args;		// Argument for command
};

// Common functions
void print(std::string message);			// Outputs a new line to STDOUT
void error(std::string message);			// Outputs new line to STDERR
std::string prompt(std::string message);		// Prompts with message and returns input from STDIN

Command str2cmd(std::string input);		// Returns which command and argument
std::vector<std::string> explode(std::string str, char delim);	// Splits string by delimiter
bool isCmd(std::string input);				// Simply evaluates whether input starts with the escape for commands
std::string c2substr(char * c, unsigned int start=0, unsigned int n=RECV_BUFFER_SIZE);	// returns substring from char array

#endif
