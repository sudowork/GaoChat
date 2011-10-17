#include "common.h"

using namespace std;

void print() {
	cout << endl;
}

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
	if (input[0] != CMD_ESCAPE) {
		c.isValid = false;
		return c;
	}

	// Split command by delimiter
	vector<string> splitCmd = explode(input);

	// Strip delimiter and store in return struct
	c.cmd = splitCmd[0].substr(1);
	// Assume valid command
	c.isValid = true;

	// get args
	splitCmd.erase(splitCmd.begin());
	c.args = splitCmd;


	// Validate command and arguments
	// IF tree for all commands (ugly I know)
	if ((c.cmd).compare(BOOTSTRAP) == 0) {
		// Check if only one argument, and is number from 1024-65535
		if (c.args.size() != 2) {
			c.isValid = false;
			return c;
		} else {
			int port;
			istringstream(c.args[0]) >> port;
			if (port < 1024 || port > 65535) {
				c.isValid = false;
				return c;
			}
		}
	} else if ((c.cmd).compare(QUIT) == 0) {
		// No argument for QUIT return quit w/ null argument
	} else if ((c.cmd).compare(MSG) == 0) {
		// Let client handle message argument
	} else if (c.cmd.compare(GETPEERS) == 0) {

	} else {
		// If does not match a command, set to invalid command
		c.isValid = false;
	}
	
	return c;
}

vector<string> explode(string str, char delim) {
	vector<string> args;

	while (str.length() > 0) {
		int pos = str.find(delim);
		if (pos < 0) {
			args.push_back(str);
			str.erase(0,str.length());
		} else {
			args.push_back(str.substr(0,pos));
			str.erase(0,pos+1);
		}
	}
	return args;
}

bool isCmd(string input) {
	return input[0] == CMD_ESCAPE;
}

string c2substr(char *c, unsigned int start, unsigned int n) {
	string sub = "";
	int length = strlen(c);
	int i = (start >= length) ? 0 : start;			// ensure start does not exceed index
	int stop = (i + n > length) ? length : i + n;	// ensure stop point does not exceed index
	for (int i = start; i < stop; i++) {
		sub += c[i];
	}
	return sub;
}

string map2str(map<string,string> m) {
	string str = "";
	map<string,string>::iterator it;

	for (it = m.begin(); it != m.end(); it++) {
		str += (*it).first + "," + (*it).second + ";";
	}
	return str;
}

IPPort parseIPPstr(string ipp) {
	IPPort i;
	i.ip = ipp.substr(0,ipp.find(':'));
	istringstream(ipp.substr(ipp.find(':')+1)) >> i.port;
	return i;
}
