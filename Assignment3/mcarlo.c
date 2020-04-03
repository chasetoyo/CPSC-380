/*
Chase Toyofuku-Souza
toyofukusouza@chapman.edu
2296478

A simple C program demonstrating the usage of threads.
It takes in the number of points as a command line argument (int)
then uses two worker threads to create random points and 
check if they are within the bounds of a circle to approximate
the value of pi.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*To pass lower and upper limit to thread function*/
struct args {
    int lower;
    int upper;
};

double hit_count = 0.0; /* this data is shared by the thread(s) */

void *runner(void *param); /* the thread */

/* Generates a double precision random number */
double random_double() 
{
	return random() / ((double)RAND_MAX + 1);
}

int main(int argc, char *argv[])
{
	pthread_t tid1, tid2; /* the thread identifier */
	pthread_attr_t attr; /* set of attributes for the thread */

	double pi;

	if (argc != 2) {
	fprintf(stderr,"usage: ./mcarlo <integer value>\n");
	return -1;
	}

	/*Set number of points user entered after we confirm that they entered something*/
	int num_points = atoi(argv[1]);

	if (num_points < 0) {
		fprintf(stderr,"Argument '%d' must be non-negative\n",atoi(argv[1]));
		return -1;
	}

	else if (num_points == 0) { //atoi returns 0 if it fails
		fprintf(stderr,"Argument '%s' must be a number\n", argv[1]);
		return -1;
	}

	/*Allocate memory for the argument structs*/
	struct args *args1 = (struct args *)malloc(sizeof(struct args));
	struct args *args2 = (struct args *)malloc(sizeof(struct args));

	/* get the default attributes */
	pthread_attr_init(&attr);

	/*Assigning the limits for the loops to divide work in half*/
	args1 -> lower = 0;
	args1 -> upper = num_points/2; 
	args2 -> lower = num_points/2;
	args2 -> upper = num_points;

	/* create the threads */
	pthread_create(&tid1,&attr,runner, (void *) args1);
	pthread_create(&tid2,&attr,runner, (void *) args2);

	/* now wait for the thread to exit */
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);

	pi = 4.0 * hit_count/num_points;
	printf("Approximated value of pi: %f\n", pi);

	/*Free memory of structs*/
	free(args1);
	free(args2);
	return 0;
}

/*
 * The thread will begin control in this function
 */
void *runner(void *param) {
	int i, upper;
	double x, y;

	int local_hit = 0; /*keep track of hits this thread found*/

	i = ((struct args*)param)->lower;
	upper = ((struct args*)param)->upper;

	for (i; i < upper; ++i) {	
	/* generate random numbers between -1.0 and +1.0 (exclusive) */
		x = random_double() * 2.0 - 1.0;
		y = random_double() * 2.0 - 1.0;

		if (sqrt(x*x + y*y) < 1.0 ) {
			++local_hit;	
		}
	}

	hit_count += local_hit; //add to the total hit count at one time

	pthread_exit(0);
}
