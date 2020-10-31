/*
Assignment 3: Smallsh
Creaor: Josh Bell
Date of Creation: October 29th
Date of Completion: ---
File: main.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int MAX_CHAR = 2048;
int MAX_CMD = 512;
int NEW_LINE_CHAR_VALUE = 10;

// ================================================================================================
/*
Struct: command
char * commandline      The users entire command command line
char * command          The users command
char * parameters		An array of the parameters
char * inputFile
char * outputFile
int backgroundValue          The value is to check if '&' is in the command if so run in the background (1 = TRUE / 0 = FALSE)
*/
struct command {
	char* commandLine;
	char* command;
	char* parameters;
	char* inputFile;
	char* outputFile;
	int backgroundValue;
};
// ================================================================================================
/*
char * getCommandLine
Reads the current line in the script and returns a char pointer of the line
Parameters: ---
Returns: char * cmdLine
*/
char* getCommandLine() {
	// Initialize Values
	char* cmdLine;
	char* currentLine = NULL;
	size_t maxChar = MAX_CHAR;

	// Setting the cmdLine to NULL and ensures it hold the correct ammount of chars
	cmdLine = calloc(MAX_CHAR + 1, sizeof(char));
	printf(": ");
	fflush(stdout);

	// Gets the current Command Line
	getline(&currentLine, &maxChar, stdin);
	cmdLine = currentLine;

	return cmdLine;
}
// ================================================================================================
/*
void processCommandLine
---
Parameters: ---
Returns: ---
*/
void processCommandLine() {
	// Initialize Value
	struct command* currCommand = malloc(sizeof(struct command));
	char* currCmdLine = calloc(MAX_CHAR + 1, sizeof(char));
	char* temp = calloc(MAX_CHAR + 1, sizeof(char));
	int parameterLength = 0;
	int commandLength = 0;
	int cmdLineLength = 0;

	// Goes through the script untill the exit command shows up
	while (strcmp(currCmdLine, "exit") - NEW_LINE_CHAR_VALUE != 0) {
		// --------------------- Fills out struct -----------------------------

		// Copys the current command line into currCmdLine and fills out struct
		strcpy(currCmdLine, getCommandLine());

		// The full command line goes into currCommand->commandLine
		currCommand->commandLine = currCmdLine;

		// Gets the length of the full command line
		cmdLineLength = strlen(currCmdLine);

		// This makes a temporary command line
		strcpy(temp, currCmdLine);

		// Finds the command by tokenizing until ' '
		char* token = strtok(temp, " ");
		currCommand->command = token;

		// Places the rest of the line in the parameters
		commandLength = strlen(currCommand->command) + 1;

		// If they are equal that means there is no parameters so skip this section
		if (cmdLineLength == commandLength - 1)
			currCommand->parameters = "\0";
		else {
			// We make token the command line then we delete the command from it
			token = currCmdLine;
			memmove(token, token + commandLength, strlen(token));
			currCommand->parameters = token;
		}

		// Searches at the last value for an &
		parameterLength = strlen(token);
		if (token[parameterLength - 2] == '&')
			currCommand->backgroundValue = 1;
		else
			currCommand->backgroundValue = 0;

		// --------------------------------------------------------------------
		// -------------------- Executes Built-In command ---------------------
		// Echo Command
		if (strcmp(currCommand->command, "echo") == 0) {
			// If there are no parameters then echo "\n"
			if (strcmp(currCommand->command, "\0") == 0) {
				char* newargv[] = { "/bin/echo", "\n", NULL };
				execv(newargv[0], newargv);
			}
			// If there are parameters then echo currCommand->parameters
			else {
				char* newargv[] = { "/bin/echo", currCommand->parameters, NULL };
				execv(newargv[0], newargv);
			}
		}
		else {
			char* newargv[] = { "/bin/echo", "THIS ISNT ECHO", NULL };
			execv(newargv[0], newargv);
		}
		// --------------------------------------------------------------------

	}
}
// ================================================================================================

// ================================================================================================
int main() {
	processCommandLine();

	return EXIT_SUCCESS;
}