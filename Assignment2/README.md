# Simple Shell

## Chase Toyofuku-Souza
> 2296478

> toyofukusouza@chapman.edu

----
### About
A simple shell interface program that takes a simple shell command as user input from the parent process then sends the simple shell command to the child process via a pipe IPC which actually executes the shell command. The program will run until the user types "q" or "quit".

### Files
- sshell.c

### Instructions
- gcc sshell.c -o sshell
- ./sshell
- Enter simple shell commands such as ls, cat, vi, etc.

### Notes
- Maximum input size is 25 characters
- Does not support commands that change directory