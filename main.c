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
#include <fcntl.h>
#include <sys/stat.h>

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
	char* tempCmdLine = calloc(MAX_CHAR + 1, sizeof(char));
	char* temp = calloc(MAX_CHAR + 1, sizeof(char));
	int parameterLength = 0;
	int commandLength = 0;
	int cmdLineLength = 0;

	pid_t smallshPID = getppid();
	pid_t spawnpid;
	int childExitMethod = -5;

	// --------------------- Fills out struct -----------------------------
	while (strcmp(currCmdLine, "exit") - NEW_LINE_CHAR_VALUE != 0) {

		// PARENT: runs thorough the filling out the struct then forks afterwards 
		// to have the child exec while waithing for the child to finsh when it does 
		// go through the the filling out again until exit is found as the cmdline

		// Copys the current command line into currCmdLine and fills out struct
		strcpy(currCmdLine, getCommandLine());

		// COMMAND LINE
		currCommand->commandLine = currCmdLine;
		strcpy(tempCmdLine, currCmdLine);

		// Checks to see if the command was starting with '#' if so apply ignore command else continue normally
		if (currCommand->commandLine[0] == '#') {
			currCommand->command = "ignore";
		}
		else {
			// Gets the length of the full command line
			cmdLineLength = strlen(currCmdLine);

			// This makes a temporary command line
			strcpy(temp, currCmdLine);

			// Finds the command by tokenizing until ' '
			// COMMAND
			char* token = strtok(temp, " ");
			currCommand->command = calloc(strlen(token) + 1, sizeof(char));
			strcpy(currCommand->command, token);

			// Places the rest of the line in the parameters
			commandLength = strlen(currCommand->command) + 1;

			// If they are equal that means there is no parameters so change name of command to echo and have parameter of "\n"
			if (token[commandLength - 2] == '\n') {
				if (token[0] == 'e') {
					currCommand->command = "echo";
					currCommand->parameters = "\n";
				}
				if (token[0] == 'l') {
					currCommand->command = "ls";
					currCommand->parameters = "\0";
				}
			}

			// We make token the command line then we delete the command from it
			else {
				strcpy(token, tempCmdLine);
				memmove(token, token + commandLength, strlen(token));
				currCommand->parameters = token;
			}

			// Searches at the last value for an &
			parameterLength = strlen(token);
			if (token[parameterLength - 2] == '&')
				currCommand->backgroundValue = 1;
			else
				currCommand->backgroundValue = 0;

			// Checks for the '$$' to change it into the PID
			if (strstr(currCommand->parameters, "$$") != NULL) {
				// PID of the smallsh into a char and storing it int cpid
				char * charPID = calloc(MAX_CHAR + 1, sizeof(char));
				sprintf(charPID, "%d", smallshPID);

				// Tokenizing the first part the parameters then cating the pid to the token
				char* moneyToken = strtok(currCommand->parameters, "$$");
				strcat(moneyToken, charPID);

				// Making the currCommand->parameters = to the parameters with $$ replaced by the PID of smallsh
				strcpy(currCommand->parameters, moneyToken);
			}
		}
		// --------------------------------------------------------------------

		// CHILD: This is where the fork would happen the child would take the data 
		// from the parent and exec while the parent waits for the exec then repeats
		// with new information for the next child

		// -------------------- Executes Built-In commands --------------------

		// Echo Command -------------------------
		if (strcmp(currCommand->command, "echo") == 0) {
			printf("%s", currCommand->parameters);
			fflush(stdout);
			//char* newargv[] = { "/bin/echo", currCommand->parameters, NULL };
			//execv(newargv[0], newargv);
		}

		// Ignore Command -----------------------
		else if (strcmp(currCommand->command, "ignore") == 0) {
			// Do nothing and ignore the line
		}

		// Ls Command ---------------------------
		else if (strcmp(currCommand->command, "ls") == 0) {
			// If the command line is just ls then run ls
			if (strcmp(currCommand->parameters, "\0") == 0) {
				//char* newargv[] = { "/bin/ls", NULL };
				//execv(newargv[0], newargv);
			}
			// If there is more to the command line that means theres output files invloved
			else {
				// Parse the parameters so just the output file is left then store that in currCommand->outputFile
				temp = currCommand->parameters;
				char* token2 = strtok(temp, "\n");
				memmove(token2, token2 + 2, strlen(token2));
				currCommand->outputFile = token2;

				// Outsource the next data into currCommand->outputFile
				//int out = open(currCommand->outputFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
				//dup2(out, 1); // http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html

				// Writing the information to be outsourced
				//char* newargv[] = { "/bin/ls", NULL };
				//execvp(newargv[0], newargv);
			}
		}
		// PWD Command --------------------------
		else if (strcmp(currCommand->command, "pwd\n") == 0) {
			// Create a buffer and the current working directory
			char* buffer2 = calloc(MAX_CHAR + 1, sizeof(char));
			char* workingPath = calloc(MAX_CHAR + 1, sizeof(char)); 
			strcpy(workingPath, getcwd(buffer2, MAX_CHAR));

			// Print it out
			printf("%s\n", workingPath);
			fflush(stdout);
		}
		// Cat Command --------------------------
		else if (strcmp(currCommand->command, "cat") == 0) {
			// Find the file name of whats going to be cated
			temp = currCommand->parameters;
			char* token2 = strtok(temp, "\n");

			//char* newargv[] = { "/bin/cat", token2, NULL };
			//execvp(newargv[0], newargv);
		}
		// Wc Command =--------------------------
		else if (strcmp(currCommand->command, "wc") == 0) {
			// Checks to see if the parameters have both '<' and '>'
			if (strchr(currCommand->parameters, '<') != NULL && strchr(currCommand->parameters, '>') != NULL) {
				// Parse the paramters to find the file name and store in currCommand->inputFile and currCommand->outputFile
				temp = currCommand->parameters;
				char* token2 = strtok(temp, "\n");
				char* token3 = calloc(strlen(token2) + 1, sizeof(char));
				strcpy(token3, token2);

				// Input file name stored in currCommand->inputFile
				memmove(token2, token2 + 2, strlen(token2));
				token2 = strtok(temp, " ");
				currCommand->inputFile = calloc(strlen(token2) + 1, sizeof(char));
				strcpy(currCommand->inputFile, token2);

				int inputLength = strlen(currCommand->inputFile);

				// Output file name stored in currCommand->outputFile
				memmove(token3, token3 + inputLength + 5, strlen(token3));
				currCommand->outputFile = calloc(strlen(token3) + 1, sizeof(char));
				strcpy(currCommand->outputFile, token3);

				// Opens input and output files
				//int in = open(currCommand->inputFile, O_RDONLY);
				//int out = open(currCommand->outputFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

				//dup2(in, 0);
				//dup2(out, 1);

				//char* newargv[] = { "wc", currCommand->inputFile, NULL };
				//execvp(newargv[0], newargv);

			}
			// Checks to see if the parameter has only the '<' and not '>'
			else if (strchr(currCommand->parameters, '<') != NULL) {
				// Parse the paramters to find the file name and store in currCommand->inputFile
				temp = currCommand->parameters;
				char* token2 = strtok(temp, "\n");
				memmove(token2, token2 + 2, strlen(token2));
				currCommand->inputFile = token2;

				// Create input
				//int in = open(currCommand->inputFile, O_RDONLY);
				//dup2(in, 0);

				//char* newargv[] = { "/bin/wc", currCommand->inputFile, NULL };
				//execvp(newargv[0], newargv);
			}

		}
		// Test Command -------------------------
		else if (strcmp(currCommand->command, "test") == 0) {
			printf("testing....\n");
			fflush(stdout);
		}
		// Status Command -----------------------
		else if (strcmp(currCommand->command, "status") == 0) {
			printf("PAR |%s|\n", currCommand->parameters);
			fflush(stdout);

			char* newargv[] = { "/bin/status", currCommand->parameters, NULL };
			execvp(newargv[0], newargv);
		}

		else if (strcmp(currCommand->command, "badfile\n") == 0) {
			printf("nonononononononononno\n");
			fflush(stdout);
		}
		// Sleep command
		else if (strcmp(currCommand->command, "sleep") == 0) {
			// Gets rid of the new line character in the parameters so it can execute
			strtok(currCommand->parameters, "\n");
			if (currCommand->backgroundValue = 1) {
				strtok(currCommand->parameters, " ");
				//char* newargv[] = { "/bin/sleep", currCommand->parameters, NULL };
				//execvp(newargv[0], newargv);
			}
			else {
				//char* newargv[] = { "/bin/sleep", currCommand->parameters, NULL };
				//execvp(newargv[0], newargv);
			}
		}
		else if (strcmp(currCommand->command, "pkill") == 0) {
			printf("we kilin\n");
			fflush(stdout);
		}
		// Cd Command to go to home dir ---------
		else if (strcmp(currCommand->command, "cd\n") == 0) {
			printf("back to home directory\n");
			fflush(stdout);

			//char* newargv[] = { "/bin/cd", NULL };
			//execvp(newargv[0], newargv);
		}
		// Cd command to go to specifc dir ------
		else if (strcmp(currCommand->command, "cd") == 0) {
			printf("we cding umm...|%s|", currCommand->parameters);
			fflush(stdout);

			//char* newargv[] = { "/bin/cd", currCommand->parameters, NULL };
			//execvp(newargv[0], newargv);
		}
		// Mkdir Command ------------------------
		else if (strcmp(currCommand->command, "mkdir") == 0) {
			printf("making the dir duh\n");
			fflush(stdout);

			mkdir(currCommand->parameters, 0777);
		}




		else {
			printf("CURR CMD |%s|\n", currCommand->command);
			fflush(stdout);
			printf("CURR PAR |%s|\n", currCommand->parameters);
			fflush(stdout);
			char* newargv[] = { "/bin/echo", "THIS ISNT ECHO OR IGNORE", NULL };
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