# File Concatenate Program

## Chase Toyofuku-Souza
> 2296478

> toyofukusouza@chapman.edu

----
### About
A C program that takes two file names as comand line arguments and concatenates the contents of the first file to the second file. The I/O is implemented using the Linux system calls *open(), close(), read(), and write()*.

### Files
- filecat.c

### Instructions
- gcc filecat.c -o filecat
- ./filecat <input file> <output file>

### Notes
A file cannot contain more than 128 characters, or it will
be truncated when copying.