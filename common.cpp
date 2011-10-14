#include "common.h"

// Print message to console
void print(string message) {
	cout << message << endl;
}

// Prompts with message, and return input
string prompt(string message) {
	string answer;
	cout << message;
	cin >> answer;
	return answer;
}
