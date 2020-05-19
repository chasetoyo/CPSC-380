# Using Threads and Mutex/Counting Semaphores

## Chase Toyofuku-Souza
> 2296478

> toyofukusouza@chapman.edu

----
### About
A C program that creates two threads. The first thread is the consumer thread that consumes the data written to a shared memory buffer. The second thread is the producer thread that “produces” the data for the shared memory buffer. In order to prevent a race condition (e.g. the consumer reading before the producer writing) use a mutex semaphore and counting semaphores to coordinate when each thread can safely write or read to/from a common shared memory region. 

### Files
- prodcon.c

### Instructions
- gcc prodcon.c -o prodcon -pthread -lrt
- ./prodcon <memory size> <number of r/w>

### Notes
- Maximum memory size is limited to 64000
- Consumer thread reads shared memory buffer of 30 bytes, calculates the checksum based on the 30 bytes, and compares that with the value stored in the shared memory buffer to ensure that data did not get corrupted
- This process happens n times (given by the 3rd command line argument)