// Common libraries
#include <iostream>
#include <string>
#include <exception>

using namespace std;

// Constants
#define	C_PORT 9001		// Client send port
#define	C_PORT_LST 9002	// Client recv port
#define S_PORT 9003		// Server send port
#define S_PORT_LST 9004	// Server recv port

// Common functions
void print(string message);
string prompt(string message);
