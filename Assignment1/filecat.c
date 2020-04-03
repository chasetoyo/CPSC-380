/*
Chase Toyofuku-Souza
toyofukusouza@chapman.edu
2296478

A simple C program that uses Linux System calls such as 
read, write, and open to concatenate the content of one 
file to another.

A file cannot contain more than 128 characters, or it will
be truncated when copying.

File names are provided via command-line arguments.
The first argument will be the name of the input file, 
followed by the name of the output file.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define ARR_SIZE 128

int main(int argc, char const *argv[])
{
    if (argc != 3) { //check if right num of args provided
        printf("You provided %i argument(s), %i required.\n", argc, 3);
        return -1;
    }

    char* data = (char *) calloc(ARR_SIZE, sizeof(char)); //allocate array that can hold 128 characters

    int fd_r = open(argv[1], O_RDONLY);
    
    if (fd_r < 0) { //if no file exists, quit
        printf("Error %i: %s\n", errno, strerror(errno));
        return -1;
    }

    int fd_w = open(argv[2], O_WRONLY | O_APPEND); //attempt to open a file to append to
    
    if (fd_w < 0) { //if opening failed, create the file then open it for writing
        printf("Error %i: %s\n", errno, strerror(errno));
        printf("Creating '%s' now.\n", argv[2]);	

        fd_w = open(argv[2], O_CREAT, S_IRUSR | S_IWUSR); //create file with read and write permissions
        close(fd_w);
        fd_w = open(argv[2], O_WRONLY,O_APPEND); //open newly created file to append to
    }

    int size = read(fd_r, data, ARR_SIZE); //only read as many characters as can fit in the buffer
    
    int amt_written = write(fd_w, data, size); //write entire array to file
    
    if (amt_written < 0) {
        printf("Error %i: %s\n", errno, strerror(errno));
        return -1;
    }

    printf("Done.\n");
    
    free(data);
    close(fd_r);
    close(fd_w);
    return 0;
}