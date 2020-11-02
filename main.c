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
#include <sys/wait.h>

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
struct command * fillCommand()
---
Parameters: ---
Returns: ---
*/
struct command* fillCommand(pid_t smallshPID) {
	// Initialize command struct
	struct command* currCommand = malloc(sizeof(struct command));
	// Initialize values
	char* currCmdLine = calloc(MAX_CHAR + 1, sizeof(char));
	char* tempCmdLine = calloc(MAX_CHAR + 1, sizeof(char));
	char* temp = calloc(MAX_CHAR + 1, sizeof(char));
	char* null = "\0";

	int parameterLength = 0;
	int commandLength = 0;
	int cmdLineLength = 0;

	// Setting all of struct to null
	currCommand->commandLine = calloc(strlen(null) + 1, sizeof(char));
	strcpy(currCommand->commandLine, null);
	currCommand->command = calloc(strlen(null) + 1, sizeof(char));
	strcpy(currCommand->command, null);
	currCommand->parameters = calloc(strlen(null) + 1, sizeof(char));
	strcpy(currCommand->parameters, null);
	currCommand->inputFile = calloc(strlen(null) + 1, sizeof(char));
	strcpy(currCommand->inputFile, null);
	currCommand->outputFile = calloc(strlen(null) + 1, sizeof(char));
	strcpy(currCommand->outputFile, null);
	currCommand->backgroundValue = 0;


	// Copys the current command line into currCmdLine and fills out struct
	strcpy(currCmdLine, getCommandLine());

	// Places the currCmdLine into the currCommand->commandLine
	currCommand->commandLine = calloc(strlen(currCmdLine) + 1, sizeof(char));
	strcpy(currCommand->commandLine, currCmdLine);
	strcpy(tempCmdLine, currCmdLine);

	// Checks to see if the command was starting with '#' if so apply ignore command else continue normally
	if (currCommand->commandLine[0] == '#') {
		currCommand->command = "ignore";
	}
	else {
		// Gets the length of the full command line
		cmdLineLength = strlen(currCmdLine);

		// This makes a temporary command line
		temp = calloc(strlen(currCmdLine) + 1, sizeof(char));
		strcpy(temp, currCmdLine);

		// Finds the command by tokenizing until ' '
		char* token = strtok(temp, " ");
		currCommand->command = calloc(strlen(token) + 1, sizeof(char));
		strcpy(currCommand->command, token);

		// Places the rest of the line in the parameters
		commandLength = strlen(currCommand->command) + 1;

		// If they are equal that means there is no parameters so change name of command to echo and have parameter of " "
		if (token[commandLength - 2] == '\n') {
			if (token[0] == 'e') {
				currCommand->command = "echo";
				currCommand->parameters = " ";
			}
			if (token[0] == 'l') {
				currCommand->command = "ls";
				currCommand->parameters = "\0";
			}
		}
		// We make token the command line then we delete the command from it
		else {
			token = calloc(strlen(tempCmdLine) + 1, sizeof(char));
			strcpy(token, tempCmdLine);
			memmove(token, token + commandLength, strlen(token));
			currCommand->parameters = calloc(strlen(token) + 1, sizeof(char));
			strcpy(currCommand->parameters, token);
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
			char* charPID = calloc(MAX_CHAR + 1, sizeof(char));
			sprintf(charPID, "%d", smallshPID);

			// Tokenizing the first part the parameters then cating the pid to the token
			char* moneyToken = strtok(currCommand->parameters, "$$");
			strcat(moneyToken, charPID);

			// Making the currCommand->parameters = to the parameters with $$ replaced by the PID of smallsh
			currCommand->parameters = calloc(strlen(moneyToken) + 1, sizeof(char));
			strcpy(currCommand->parameters, moneyToken);
		}

	}

	return currCommand;
}
// ================================================================================================
/*
void processCommandLine
---
Parameters: ---
Returns: ---
*/
void processCommandLine(struct command* currCommand, int smallshPID) {
	// Initialize a temp char 
	char* temp = calloc(MAX_CHAR + 1, sizeof(char));
	pid_t spawnpidsleep = -5;

	// Echo Command -------------------------------------------------------------------------------
	if (strcmp(currCommand->command, "echo") == 0) {
		// Tokenizes the parameters untill the '\n' and echos it out unless thats the only value
		if (strcmp(currCommand->parameters, " ") != 0) {
			char* echoToken = strtok(currCommand->parameters, "\n");
			currCommand->parameters = calloc(strlen(echoToken) + 1, sizeof(char));
			strcpy(currCommand->parameters, echoToken);
		}
		// Executes echo with the given parameters
		char* newargv[] = { "/bin/echo", currCommand->parameters, NULL };
		execv(newargv[0], newargv);
	}

	// Ignore Command -----------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "ignore") == 0) {
		// Do nothing and ignore the line
		exit(0);
	}

	// Ls Command ---------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "ls") == 0) {
		// If the command line is just ls then run ls
		if (strcmp(currCommand->parameters, "\0") == 0) {
			char* newargv[] = { "/bin/ls", NULL };
			execv(newargv[0], newargv);
		}
		// If there is more to the command line that means theres output files invloved
		else {
			// Parse the parameters so just the output file is left then store that in currCommand->outputFile
			temp = currCommand->parameters;
			char* token2 = strtok(temp, "\n");
			memmove(token2, token2 + 2, strlen(token2));
			currCommand->outputFile = token2;

			// Outsource the next data into currCommand->outputFile
			int out = open(currCommand->outputFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
			dup2(out, 1); // http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html

			// Writing the information to be outsourced
			char* newargv[] = { "/bin/ls", NULL };
			execvp(newargv[0], newargv);
		}
	}

	// PWD Command --------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "pwd\n") == 0) {
		// Create a buffer and the current working directory
		char* buffer2 = calloc(MAX_CHAR + 1, sizeof(char));
		char* workingPath = calloc(MAX_CHAR + 1, sizeof(char));
		strcpy(workingPath, getcwd(buffer2, MAX_CHAR));

		// Print it out
		printf("%s", workingPath);
		fflush(stdout);
		exit(0);
	}
	// Cat Command --------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "cat") == 0) {
		// Find the file name of whats going to be cated
		temp = currCommand->parameters;
		char* token2 = strtok(temp, "\n");

		char* newargv[] = { "/bin/cat", token2, NULL };
		execvp(newargv[0], newargv);
	}
	// Wc Command =--------------------------------------------------------------------------------
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
			int in = open(currCommand->inputFile, O_RDONLY);
			int out = open(currCommand->outputFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);

			dup2(in, 0);
			dup2(out, 1);

			char* newargv[] = { "wc", currCommand->inputFile, NULL };
			execvp(newargv[0], newargv);

		}
		// Checks to see if the parameter has only the '<' and not '>'
		else if (strchr(currCommand->parameters, '<') != NULL) {
			// Parse the paramters to find the file name and store in currCommand->inputFile
			temp = currCommand->parameters;
			char* token2 = strtok(temp, "\n");
			memmove(token2, token2 + 2, strlen(token2));
			currCommand->inputFile = token2;

			// Create input
			int in = open(currCommand->inputFile, O_RDONLY);
			dup2(in, 0);

			char* newargv[] = { "/bin/wc", currCommand->inputFile, NULL };
			execvp(newargv[0], newargv);
		}
		exit(0);
	}
	// Test Command -------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "test") == 0) {
		char* newargv[] = { "/bin/test", currCommand->parameters, NULL };
		execvp(newargv[0], newargv);
	}
	// Status Command -----------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "status") == 0) {

	}
	// Bad file
	else if (strcmp(currCommand->command, "badfile\n") == 0) {
		FILE* file;
		file = fopen(currCommand->command, "r");
		if (file == NULL)
			fprintf(stderr, "file could not be read for input\n");
	}
	// Sleep command ------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "sleep") == 0) {
		// Gets rid of the new line character in the parameters so it can execute
		strtok(currCommand->parameters, "\n");

		if (currCommand->backgroundValue == 1) {
			strtok(currCommand->parameters, " ");

			pid_t ogPID;
			ogPID = getpid();

			spawnpidsleep = fork();
			pid_t bPID;
			bPID = getpid();
			if (bPID - ogPID == 1) {
				printf("Background PID is |%d|\n", bPID);
				fflush(stdout);
				char* newargv[] = { "/bin/sleep", currCommand->parameters, NULL };
				execvp(newargv[0], newargv);
			}
		}
		else {
			char* newargv[] = { "/bin/sleep", currCommand->parameters, NULL };
			execvp(newargv[0], newargv);
		}
		exit(0);
	}
	// Pkill Command ------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "pkill") == 0) {
		char* pkillToken = strtok(currCommand->parameters, "\n");
		currCommand->parameters = calloc(strlen(pkillToken) + 1, sizeof(char));
		strcpy(currCommand->parameters, pkillToken);

		char* newargv[] = { "/bin/pkill", currCommand->parameters, NULL };
		execvp(newargv[0], newargv);
	}
	// Cd Command to go to home dir ---------------------------------------------------------------
	else if (strcmp(currCommand->command, "cd\n") == 0) {
		// Ignore done in main
	}
	// Cd command to go to specifc dir ------------------------------------------------------------
	else if (strcmp(currCommand->command, "cd") == 0) {
		// Ignore done in main
	}
	// Mkdir Command ------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "mkdir") == 0) {
		mkdir(currCommand->parameters, 0777);
		printf("\n");
		fflush(stdout);
	}
	// Date Command -------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "date\n") == 0) {
		char* newargv[] = { "/bin/date", NULL };
		execvp(newargv[0], newargv);
	}
	// Kill Command -------------------------------------------------------------------------------
	else if (strcmp(currCommand->command, "kill") == 0) {
		char* PIDc = calloc(MAX_CHAR + 1, sizeof(char));
		sprintf(PIDc, "%d", smallshPID);

		printf("|%s|", PIDc);
		fflush(stdout);

		char* newargv[] = { "/bin/kill", currCommand->parameters, PIDc, NULL };
		execvp(newargv[0], newargv);
	}

	// CATCH --------------------------------------------------------------------------------------
	else {
		printf("CURR CMD |%s|\n", currCommand->command);
		fflush(stdout);
		printf("CURR PAR |%s|\n", currCommand->parameters);
		fflush(stdout);
		char* newargv[] = { "/bin/echo", "THIS ISNT RIGHT", NULL };
		execv(newargv[0], newargv);
	}
}
// ================================================================================================
/*
void processCommandLine
---
Parameters: ---
Returns: ---
*/
void exitCommand() {

	printf("exit program\n\n");
	fflush(stdout);
}
// ================================================================================================
/*
void cdCommand
Changes the current working dirrectory if the currCommand fufills the if statements
Parameters: struct command* currCommand
Returns: ---
*/
void cdCommand(struct command* currCommand) {
	// Initialize variables
	char* buffer2 = calloc(MAX_CHAR + 1, sizeof(char));
	char* currDIR = calloc(MAX_CHAR + 1, sizeof(char));

	// Changes to home dir 
	strcpy(currDIR, getcwd(buffer2, MAX_CHAR));
	if (strcmp(currCommand->command, "echo") != 0)
		if (strcmp(currCommand->command, "pwd\n") != 0)
			if (strcmp(currCommand->command, "cd\n") == 0)
				chdir(getenv("HOME"));

	// Changes to currDirr ++ currCommand->parameters
	if (strcmp(currCommand->command, "cd") == 0) {
		strcat(currDIR, "/");
		strcat(currDIR, currCommand->parameters);
		chdir(currDIR);
	}
}
// ================================================================================================
int main() {
	// Initialize Values
	struct command* currCommand = malloc(sizeof(struct command));
	char* begin = "this Is The Begining no information yet";

	// Setting the structs command line to something so it can ceck its not "exit\n"
	currCommand->commandLine = calloc(strlen(begin) + 1, sizeof(char));
	strcpy(currCommand->commandLine, begin);

	// Finding the current working dir and saving it into smallshDIR
	char* buffer = calloc(MAX_CHAR + 1, sizeof(char));
	char* smallshDIR = calloc(MAX_CHAR + 1, sizeof(char));
	strcpy(smallshDIR, getcwd(buffer, MAX_CHAR));

	pid_t smallshPID = getppid();
	pid_t spawnpid = -5;
	int childStatus = 0;
	int childPid = 0;

	while (strcmp(currCommand->commandLine, "exit") - NEW_LINE_CHAR_VALUE != 0) {
		// Fill out the command struct with the correct values of the current line
		currCommand = fillCommand(smallshPID);

		// Activates CD if needed
		cdCommand(currCommand);

		// Fork - PARENT |Reads the current command| and CHILD |executes the command line|
		spawnpid = fork();
		switch (spawnpid) {
		case -1:
			perror("fork() failed!");
			exit(1);
			break;
		case 0:
			processCommandLine(currCommand, smallshPID);
			exit(0);
		default:
			childPid = wait(&childStatus);
			if (strcmp(currCommand->command, "pwd\n") == 0) {
				printf("\n");
				fflush(stdout);
			}
			break;
		}
	}

	exitCommand();

	return EXIT_SUCCESS;
}