# Disk Scheduler

## Chase Toyofuku-Souza
> 2296478

> toyofukusouza@chapman.edu

----
### About
A C program that implements the following disk-scheduling algorithms:
- FCFS
- SSTF
- SCAN
- C-SCAN
- LOOK
- C-LOOK
The program services a disk with 5,000 cylinders numbered 0 to 4,999. The program will read a file that contains a series of 1000 cylinder requests and services them according to each of the algorithms listed above. The program is to report the total amount of head movement required by each algorithm as a summary at the end of the program.

### Files
- diskScheduler.c
- cylinder.txt

### Instructions
- gcc vmmgr.c -o vmmgr
- ./vmmgr <backing store file> <address file>

### Notes
The program will take a few seconds to do SSTF, due to the way that I programmed it. To find the closestcylinder that hasn't been visited, it has two for loops that go left and right to search for the closest cylinder on each side that hasn't been visited, and compares the distances to choose the closest one.