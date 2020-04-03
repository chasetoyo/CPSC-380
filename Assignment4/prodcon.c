/*
Chase Toyofuku-Souza
toyofukusouza@chapman.edu
2296478

A C program that demonstrates the usage of semaphores
to protect the critical section between two competing
threads. There is a consumer thread that reads from
the shared memory buffer, and a producer thread that writes
to a shared memory buffer. A checksum is used to ensure
that no data is corrupted when reading and writing.

NOTES
I did not know where to use a mutex, I instead used
two binary semaphores, one for the reader and one
for the writer. Each thread released the semaphore
for the other one which allowed them to synchronize. 

Compile the program with gcc prodcon.c -o prodcon -pthread -lrt
then run using ./prodcon <memory size> <number of r/w>
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define MAX_SIZE 64000 /*max size that user can input*/

unsigned int ip_checksum(char *data, int length);
void *write_data(void *arg);
void *read_data(void *arg);
pthread_mutex_t work_mutex; /* protects both work_area and time_to_exit */

/*declare global variables*/
int memsize, ntimes, total_blocks, shm_fd;
sem_t bin_sem, bin_sem2;
const char* name = "chase"; /*identifier of shared memory*/

int main(int argc, char const *argv[])
{
	if (argc != 3) { //check if right num of args provided
        printf("You provided %i argument(s), %i required.\n", argc, 3);
        return -1;
    }

    /*initialize the shared mem size and num
    of times data is read/written */
    memsize = atoi(argv[1]);
    ntimes = atoi(argv[2]);
    total_blocks = memsize / 32;
	int time_to_exit = 0;

	if (memsize > MAX_SIZE) {
		printf("Err. Memory size is too large, limit to less than 64k.\n");
		return -1;
	}
	if (memsize % 32 != 0) {
		printf("Err. Memory size (%s) should be a multiple of 32.\n", argv[1]);
		return -1;
	}
	if (memsize == 0) { //atoi returns 0 if it fails
		fprintf(stderr,"Err. Memory size (%s) must be a number.\n", argv[1]);
		return -1;
	}
	if (ntimes <= 0) {
		printf("Err. Number of reads/writes (%s) must be positive.\n", argv[2]);
    	return -1;
	}

    pthread_t prod, cons;
    void *thread_result;
    int res; 

	/* create the shared memory segment */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	/* configure the size of the shared memory segment */
	ftruncate(shm_fd, memsize);

    // res = pthread_mutex_init(&work_mutex, NULL); /*initialize mutex sempahore*/
    // if (res != 0) {
    //     perror("Mutex initialization failed");
    //     exit(EXIT_FAILURE);
    // }

    res = sem_init(&bin_sem, 0, 0); /*initialize binary sempahore*/
    if (res != 0) {
        perror("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }

    res = sem_init(&bin_sem2, 0, 0); /*initialize binary sempahore*/
    if (res != 0) {
        perror("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }

    sem_post(&bin_sem); /*post semaphore to allow writer to start*/

    res = pthread_create(&prod, NULL, write_data, NULL);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_create(&cons, NULL, read_data, NULL);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_join(prod, &thread_result); /*join worker threads*/
    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }

    res = pthread_join(cons, &thread_result);
    if (res != 0) {
        perror("Thread join failed");
        exit(EXIT_FAILURE);
    }

    // pthread_mutex_destroy(&work_mutex); /*destroy bin semaphore and mutex*/
    sem_destroy(&bin_sem);
    sem_destroy(&bin_sem2);
	return 0;
}

void *write_data(void *param) 
{
	for (int i = 0; i < ntimes; ++i) {
		sem_wait(&bin_sem); /*wait until writing semaphore is released*/
		unsigned short cksum = 0;
		int shm_fd, rand_int, i;
		char curr_num[50]; /*to store the random num*/
		char total_num[100] = ""; /*store entire sequence*/
		void *ptr;

		/*open the shared memory segment */
		shm_fd = shm_open(name, O_RDWR, 0666);

		/* now map the shared memory segment in the address space of the process */
		ptr = mmap(0,memsize, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
		if (ptr == MAP_FAILED) {
			printf("Map failed\n");
			exit(-1);
		}

		i = 0;
		while (i < 30) { /*generate 28 bytes of data*/
			rand_int = rand() % 256;
			sprintf(curr_num, "%i", rand_int); /*cast int to string*/
			sprintf(ptr, "%s", curr_num); /*store in shared memory*/
			strncat(total_num, curr_num, strlen(curr_num)); /*add it to local "total_num" to do checksum*/
			ptr += strlen(curr_num);
			i += strlen(curr_num); /*increment counter of bytes created*/
		}

		long unsigned int csum = ip_checksum(total_num, strlen(total_num));
		sprintf(ptr, "%lx", csum); /*write checksum to the last 2 bytes of shared memory*/
		ptr += 8; /*length of the checksum*/
		printf("Producer wrote '%s%lx' to buffer.\n", total_num, csum);
		sem_post(&bin_sem2); /*post semaphore to let consumer read*/
	}

	pthread_exit(NULL);
}

void *read_data(void *param) 
{
	for (int i = 0; i < ntimes; ++i) {
		sem_wait(&bin_sem2); /*wait for data to be written*/
		char curr_num[50]; /*to store checksum*/
		int shm_fd;
		char *ptr;

		shm_fd = shm_open(name, O_RDONLY, 0666);
		if (shm_fd == -1) {
			printf("shared memory failed\n");
			exit(-1);
		}

		/* now map the shared memory segment in the address space of the process */
		ptr = mmap(0,memsize, PROT_READ, MAP_SHARED, shm_fd, 0);
		if (ptr == MAP_FAILED) {
			printf("Map failed\n");
			exit(-1);
		}

		printf("Consumer read  '%s' from buffer.\n", ptr);

		const char *csum_cmp = &ptr[strlen(ptr)-8]; /*get last 2 "bytes" of buffer (4byte int*2bytes=8)*/
		long unsigned int csum = ip_checksum(ptr, strlen(ptr)-8);

		sprintf(curr_num, "%lx", csum); /*cast calculated checksum to a string to compare to expected checksum*/
	
		if (strcmp(csum_cmp, curr_num) != 0) { /*if checksums did not match*/
			printf("Checksum error.\nExpected: %s\nCalculated: %s\n", csum_cmp, curr_num);
			shm_unlink(name);
			exit(-1);
		}

		sem_post(&bin_sem); /*wait for data to be written*/
	}

	/* remove the shared memory segment */
	if (shm_unlink(name) == -1) {
		printf("Error removing %s\n",name);
		exit(-1);
	}

	pthread_exit(NULL);
}


unsigned int ip_checksum(char *data, int length) 
{
    unsigned int sum = 0xffff;
    unsigned short word;
    int i;
    
    // Handle complete 16-bit blocks.
    for (i = 0; i+1<length; i+=2) {
        memcpy(&word, data+i, 2);
        sum += word;
        if (sum > 0xffff) {
            sum -= 0xffff;
        }
    }
   
    // Handle any partial block at the end of the data.
    if (length&1) {
        word=0;
        memcpy(&word, data+length-1, 1);
        sum += word;;
        if (sum > 0xffff) {
            sum -= 0xffff;
        }
    }

    // Return the checksum
    return ~sum;
 }