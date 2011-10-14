#include "common.h"

using namespace std;

// Print message to STDOUT
void print(string message) {
	cout << message << endl;
}

// Print error to STDERR
void error(string message) {
	cerr << message << endl;
	if (errno != 0) {
		cerr << "    ErrNo. " << errno << ": " << strerror(errno) << endl;
	}
}

// Prompts with message, and return input
string prompt(string message) {
	string answer;
	cout << message;
	cin >> answer;
	return answer;
}

Command str2cmd(string input) {
	Command c;
	// return empty struct if not properly formed command (no escape char)
	if (input[0] != CMD_ESCAPE) return c;

	// Strip delimiter and store in return struct
	c.cmd = input.substr(1);
	// Assume valid command
	c.isValid = true;

	// get args
	//explode(input

	// Validate command and arguments
	// IF tree for all commands (ugly I know)
	if ((c.cmd).compare(BOOTSTRAP) == 0) {

	} else if ((c.cmd).compare(QUIT) == 0) {
		// No argument for QUIT return quit w/ null argument
	} else {
		// If does not match a command, set to invalid command
		c.isValid = false;
	}
	
	return c;
}

bool isCmd(string input) {
	return input[0] == CMD_ESCAPE;
}
