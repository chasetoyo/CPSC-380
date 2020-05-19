/*
Chase Toyofuku-Souza
toyofukusouza@chapman.edu
2296478

A C program that simulates the various disk-scheduling
algorithms, particularly:
	fcfs
	sstf
	look
	clook
	scan
	cscan

It will output the total number of head movements for each
algorithm given a starting head position and a file
containing 1000 lines of cylinders between 0 and 4,999.

Instructions:
gcc diskScheduler.c -o diskScheduler
./vdiskScheduler <initial head position> <cylinder file>

Notes:
The program will take a few seconds to do SSTF, due 
to the way that I programmed it. To find the closest
cylinder that hasn't been visited, it has two for loops
that go left and right to search for the closest cylinder
on each side that hasn't been visited, and compares the
distances to choose the closest one. 

My program output after running: "./diskScheduler 50 cylinder.txt"

fcfs: 1633153
sstf: 10716
scan: 5049
cscan: 4998
look: 9948
clook: 9997
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

const int MIN_CYL = 0;
const int MAX_CYL = 5000;
const int NUM_REQUESTS = 10;

int* find_closest(int head, int requests[]) {
	/*set closest distance to be max size of cylinders
	so first value it reads will automatically become the
	first shortest distance*/
	int closest_distance = MAX_CYL;
	int index_closest = -1;

	int *result = (int*) malloc(2 * sizeof(int));

	for (int i = 0; i < NUM_REQUESTS; ++i) {
		int distance = head - requests[i];

		/*account for negative distances*/
		if (distance < 0) {
			distance = distance * -1;
		}
		
		/*dont count the head as a valid shortest distance*/
		else if (distance == 0) {
			continue;
		}

		/*if its the shortest so far, save distance and index*/
		if (distance < closest_distance) {
			closest_distance = distance;
			index_closest = i;
		}

		distance = -1;

	}
	result[0] = index_closest;
	result[1] = closest_distance;
	return result;
}

/*FCFS - reads in requests as they are submitted*/
int fcfs(int head, int requests[]) {
	int currentHead = head;
	int movement = 0;
	int distance = 0;
	for (int i = 0; i < NUM_REQUESTS; ++i) {
		distance = currentHead - requests[i];
		if (distance < 0) {
			distance = distance * -1;
		}
		movement += distance;
		currentHead = requests[i];
		distance = 0;
	}
	return movement;
}

int sstf(int head, int requests[]) {
	int currentHead = head;
	int tempHead = head;
	int tempHead2 = head;

	int movement = 0;
	int distance = 0;
	
	/*keep track of visited cylinders by
	making an array of same size as requests
	1 if visited, 0 else*/
	int visited[NUM_REQUESTS];
	for (int i = 0; i < NUM_REQUESTS; ++i) {
		visited[i] = 0;
	}

	for (int i = 0; i < NUM_REQUESTS; ++i) {
		
		/*two arrays to hold index & distance from find_closest()*/
		int *arr = (int*) malloc(2 * sizeof(int));
		int *arr2 = (int*) malloc(2 * sizeof(int));

		/*attempt to find the index of closest cylinder*/
		arr = find_closest(currentHead, requests);

		int index2, index = arr[0];
		int dist2, dist = arr[1];

		/*in the case that the closest was already visited*/
		while (visited[index] == 1) {
			/*seek in both directions
			this is my way of finding the 
			"next" closest cylinder
			*/
			arr = find_closest(--tempHead, requests);
			arr2 = find_closest(++tempHead2, requests);

			index = arr[0];
			dist = arr[1];

			index2 = arr2[0];
			dist2 = arr2[1];

			/*closest cylinder will be whichever has less distance*/
			if (dist2 < dist)
				index = index2;
		}

		/*mark as visited*/
		visited[index] = 1;
		distance = currentHead - requests[index];

		if (distance < 0) {
			distance = distance * -1;
		}

		movement += distance;
		/*reset all heads to current head value*/
		currentHead = requests[index];
		tempHead = currentHead;
		tempHead2 = currentHead;

		distance = -1;
		free(arr);
		free(arr2);
	}

	return movement;
}

/*SCAN - scans left and then right from head position*/
int scan(int head, int requests[]) {
	int currentHead = head;
	int movement = 0;
	int distance = 0;

	/*scan left from head to 0*/
	for (int i = head; i >= MIN_CYL; --i) {
		for (int j = 0; j < NUM_REQUESTS; ++j) {
			if (requests[j] == i) {
				distance = currentHead - requests[j];
				movement += distance;
				currentHead = i;
				distance = 0;

				break;
			}
		}
	}
	
	/*distance from curent head to 0 is 
	just value of currentHead*/
	movement += currentHead;
	currentHead = 0;

	/*scan to max from head*/
	for (int i = head; i < MAX_CYL; ++i) {
		for (int j = 0; j < NUM_REQUESTS; ++j) {
			if (requests[j] == i) {
				distance = requests[j] - currentHead;
				movement += distance;
				currentHead = i;
				distance = 0;

				break;
			}
		}
	}
	return movement;
}

int cscan(int head, int requests[]) {
	int currentHead = head;
	int movement = 0;
	int distance = 0;

	/*scan from head to max*/
	for (int i = head; i < MAX_CYL; ++i) {
		for (int j = 0; j < NUM_REQUESTS; ++j) {
			if (requests[j] == i) {
				distance = requests[j] - currentHead;
				movement += distance;
				currentHead = i;
				distance = 0;
				break;
			}
		}
		/*reached the end of the cylinders
		distance from last cylinder to end is
		added to movement */
		if (i == MAX_CYL-1) {
			distance = i - currentHead;
			movement += distance;
			currentHead = i;
			distance = 0;
		}
	}

	/*move back to 0 to scan right -> */
	currentHead = 0;

	/*scan right from 0 to initial head*/
	for (int i = 0; i < head; ++i) {
		for (int j = 0; j < NUM_REQUESTS; ++j) {
			if (requests[j] == i) {
				distance = requests[j] - currentHead;
				movement += distance;
				currentHead = i;
				distance = 0;
				break;
			}
		}
	}

	return movement;
}

int look(int head, int requests[]) {
	int currentHead = head;
	int movement = 0;
	int distance = 0;

	/*scan to max from head*/
	for (int i = head; i < MAX_CYL; ++i) {
		for (int j = 0; j < NUM_REQUESTS; ++j) {
			if (requests[j] == i) {
				distance = requests[j] - currentHead;

				movement += distance;
				currentHead = i;
				distance = 0;

				break;
			}
		}
	}

	/*scan left from head*/
	for (int i = head; i >= 0; --i) {
		for (int j = 0; j < NUM_REQUESTS; ++j) {
			if (requests[j] == i) {
				distance = currentHead - requests[j];

				movement += distance;
				currentHead = i;
				distance = 0;

				break;
			}
		}
	}
	return movement;
}

int clook(int head, int requests[]) {
	int currentHead = head;
	int movement = 0;
	int distance = 0;
	int first = 0;

	/*scan from head to max*/
	for (int i = head; i < MAX_CYL; ++i) {
		for (int j = 0; j < NUM_REQUESTS; ++j) {
			if (requests[j] == i) {
				distance = requests[j] - currentHead;
				movement += distance;
				currentHead = i;
				distance = 0;
				break;
			}
		}
	}

	/*scan right from 0 to initial head*/
	for (int i = 0; i < head; ++i) {
		for (int j = 0; j < NUM_REQUESTS; ++j) {
			if (requests[j] == i) {
				distance = requests[j] - currentHead;
				if (distance < 0)
					distance = distance * -1;
				movement += distance;
				currentHead = i;
				distance = 0;
				break;
			}
		}
	}

	return movement;
}

int main(int argc, char const *argv[])
{
	// FILE *cylinder_requests;
	// char line[32];
	// int i = 0;
	// int address = -1;

	// if (argc != 3) { //check if right num of args provided
	//     printf("You provided %i argument(s), %i required.\n", argc, 3);
	//     return -1;
	// }

	// int head = atoi(argv[1]);
	// if (head <= 0) {
	// 	printf("Enter a non-negative number for initial cylinder position.\n");
	// 	return -1;
	// }


	// cylinder_requests = fopen(argv[2], "r");
	// if (cylinder_requests == NULL) {
	// 	 printf("Error %i: %s\n", errno, strerror(errno));
	// 	 return -1;
	// }

	// int *arr = (int*) malloc(NUM_REQUESTS * sizeof(int));

	// /*read line by line and store value in arr*/
	// while(fgets(line, 32, cylinder_requests)) {
	// 	address = atoi(line);
	// 	if (i >= NUM_REQUESTS) {
	// 		break;
	// 	}
	// 	arr[i] = address;
	// 	i++;
	// }

	// fclose(cylinder_requests);
	int arr[10] = {2069, 1212, 2296, 2800, 544, 1618, 356, 1523, 4965, 3681};
	int head = 2150;
	int res = fcfs(head, arr);
	printf("fcfs: %i\n", res);

	res = sstf(head, arr);
	printf("sstf: %i\n", res);

	res = scan(head, arr);
	printf("scan: %i\n", res);

	res = cscan(head, arr);
	printf("cscan: %i\n", res);

	res = look(head, arr);
	printf("look: %i\n", res);

	res = clook(head, arr);
	printf("clook: %i\n", res);

	// free(arr);
	return 0;
}