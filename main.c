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
	printf("\n: ");
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
		// Copys the current command line into currCmdLine and fills out struct
		strcpy(currCmdLine, getCommandLine());

		// Gets the length of the full command line
		cmdLineLength = strlen(currCmdLine);

		// Fills in the command line
		currCommand->commandLine = currCmdLine;

		printf("The cmdLine is: %s", currCommand->commandLine);
		fflush(stdout);

		// This makes a temporary command line
		strcpy(temp, currCmdLine);

		// Finds the command by tokenizing until ' '
		char* token = strtok(temp, " ");
		currCommand->command = token;

		printf("The command is: %s", currCommand->command);
		fflush(stdout);

		// Places the rest of the line in the parameters
		commandLength = strlen(currCommand->command) + 1;

		// If they are equal that means there is no parameters so skip this section
		if (cmdLineLength == commandLength - 1)
			currCommand->parameters = NULL;
		else {
			token = currCmdLine;
			memmove(token, token + commandLength, strlen(token));
			currCommand->parameters = token;
			printf("\nThe parameters are: %s\n", currCommand->parameters);
			fflush(stdout);
		}

		// Searches at the last value for an &
		parameterLength = strlen(token);
		if (token[parameterLength - 2] == '&')
			currCommand->backgroundValue = 1;
		else
			currCommand->backgroundValue = 0;

		printf("\nThe background value is: %d\n\n", currCommand->backgroundValue);
		fflush(stdout);

	}
}
// ================================================================================================

// ================================================================================================
int main() {
	processCommandLine();

	return EXIT_SUCCESS;
}