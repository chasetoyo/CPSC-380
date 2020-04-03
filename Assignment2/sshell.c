/*
Chase Toyofuku-Souza
toyofukusouza@chapman.edu
2296478

A simple C program demonstrating the usage of forks and pipes
to communicate data between processes via a simple linux shell.

Command that user enters must be less than 25 characters
or it will be truncated, however this shouldn't be an issue.

Type 'q' or 'quit' to exit the shell.
*/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 25
#define READ_END	0
#define WRITE_END	1

int main()
{
	pid_t pid;
	int fd[2];
	char write_msg[BUFFER_SIZE];
	char read_msg[BUFFER_SIZE];

	/* create the pipe */
	if (pipe(fd) == -1) {
		fprintf(stderr,"Pipe failed");
		return 1;
	}

	pid = fork();

	if (pid < 0) {
		fprintf(stderr, "Fork failed");
		return 1;
	}

	else if (pid == 0) { //this one is the child
		/* close the unused end of the pipe */
		close(fd[WRITE_END]);
		while (1) {
			/* read from the pipe */
			read(fd[READ_END], read_msg, BUFFER_SIZE);
			int quit = strcmp(read_msg, "\0");

			if (quit != 0) {
				system(read_msg);
				printf("%s", "osh> ");
				fflush(stdout); //printf wont get called unless buffer is flushed
			}

			else { //when quit is read
				close(fd[READ_END]);
				break;
			}
		}
	}

	else { //this one is the adult
		int quit1 = 1;
		int quit2 = 1;

		close(fd[READ_END]);
		printf("osh> ");

		while (quit1 != 0 && quit2 != 0) {
			fgets(write_msg, BUFFER_SIZEq, stdin);
			write_msg[strlen(write_msg)-1] = '\0'; //get rid of newline character for comparison;
			
			quit1 = strcmp(write_msg, "q");
			quit2 = strcmp(write_msg, "quit");

			if (quit1 == 0 || quit2 ==0 ) {
				write_msg[0] = '\0';
				write(fd[WRITE_END], write_msg, strlen(write_msg)+1); //send a null character to the child
				close(fd[WRITE_END]);
			}

			write(fd[WRITE_END], write_msg, strlen(write_msg)+1); 
		}

		close(fd[WRITE_END]);
	}
	return 0;
}