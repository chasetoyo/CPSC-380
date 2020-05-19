/*
Chase Toyofuku-Souza
toyofukusouza@chapman.edu
2296478

A C++ program that simulates a Rate Monotonic real-scheduler. The
scheduler creates n number of threads, then simulates the scheduling
of those threads using posix based semaphores. The scheduler will post
a semaphore based upon the RM scheduling algorithm. A clock tick is simulated
through one iteration of a loop. All tasks are periodic and released at time 0.

NOTES
Compile the program with g++ -pthread rmsched.cc -o rmsched
then run using ./rmsched <nperiods> <task set> <schedule>

The format of the task set schedule is 
task id,wcet,period
task id2,wcet2,period2
.
.
.
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <algorithm>
#include <cstring>
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
using namespace std;

struct task {
	string id;
	double wcet;
	double period;
	int ticks_remaining;

	bool operator < (const task& t) const {
        return (period < t.period);
    }
};

string get_field(string line, int num) {
	int curr_field = 1;
	string field;
	int len = line.length();
	for (int i = 0; i < len; ++i) {
		/*if we are on the right section of the csv*/
		if (curr_field == num) {
			if (line[i] != ',')
				field += line[i];
		}
		/*if we found a comma*/
		if (line[i] == ',') {
			curr_field++;
			if (curr_field > num)
				break;
		}
	}
	return field;
}

/*increment lcm by 1 until it evenly divides by inputs*/
int lcm(int i, int j) {
	int lcm = 1;

	if (i > j)
		lcm = i;
	else
		lcm = j;

	while (true) {
		if (lcm % i == 0 && lcm % j == 0)
            break;
        else
            ++lcm;	
	}
	return lcm;
}

/*finds utilization bounds and utilization
to determine if tasks are schedulable*/
bool is_schedulable(vector <task> the_vec) {
	double util = 0.0;
	double max_util = 0.7;
	double sz = the_vec.size();

	/*get cpu utilization for all tasks*/
	for (int i = 0; i < sz; ++i) {
		task the_task = the_vec[i];
		util += the_task.wcet/the_task.period;
	}

	if (util < max_util) {
		return true;
	}
	else {
		return false;
	}
}

int hperiod = 1;
int nperiods = 0;
int num_tasks = 0;
int res;
int running = 1;

sem_t parent;

vector <task> task_vec;
vector <sem_t> sem_vec;
vector <pthread_t> thread_vec;

queue <string*> output_task;
queue <string*> output_time;
ofstream out_file;

void *func(void *arg);

int main(int argc, char const *argv[])
{
	if (argc != 4) {
		cout << argc-1 << " arguments provided, 3 required." << endl;
		return -1;
	}

	/* validate first input (# of periods) */
	int len = strlen(argv[1]);
	for (int i = 0; i < len; ++i) {
		if (!isdigit(argv[1][i])){
			cout << argv[1] << ": please enter a valid number" << endl;
			return -1;
		}
	}
	nperiods = atoi(argv[1]);

	/*open task set/schedule file and quit if it doesnt exist*/
	ifstream in_file;
	in_file.open(argv[2]);
	if (!in_file.is_open())
		perror(argv[2]);

	/*open output file*/
	out_file.open(argv[3]);
	if (!out_file.is_open())
		perror(argv[3]);

	/*read task set file and create tasks*/
	string line;
	while (getline(in_file, line)) {
		/*dynamically allocate mem for new task*/
		task *t = new task;

		/*assign proper values to struct task*/
		t->id = get_field(line, 1);
		t->wcet = stoi(get_field(line, 2));
		t->period = stoi(get_field(line, 3));
		t->ticks_remaining = stoi(get_field(line, 2));
		/*insert new task into vector*/
		task_vec.push_back(*t);
	}


	/*check schedulability before continuing*/
	if (!is_schedulable(task_vec)) {
		cout << "Tasks are not schedulable" << endl;
		return -1;
	}

	/*reorder the vector based on period (shortest to longest)*/
	sort(task_vec.begin(), task_vec.end());

	num_tasks = task_vec.size();

	/*  1. find lcm of each number to get lcm of all numbers
		2. create and init a semaphore for each task
		3. create and init a thread for each task */
	for (int i = 0; i < num_tasks; ++i) {
		hperiod = lcm(hperiod, task_vec[i].period);

		sem_t *sem_temp = new sem_t;
		res = sem_init(sem_temp, 0, 0); /*initialize binary sempahore*/
		sem_vec.push_back(*sem_temp);
		if (res != 0) {
		    perror("Semaphore initialization failed");
		    return -1;
		}

		pthread_t *thread_temp = new pthread_t;
		res = pthread_create(thread_temp, NULL, func, (void*) (intptr_t) i);
		// sleep(1);
		thread_vec.push_back(*thread_temp);
		if (res != 0) {
		    perror("Thread creation failed");
		    return -1;
		}
	}

	/*initialize binary sempahore*/
	res = sem_init(&parent, 0, 0);

	/*assign curr_task to index of highest priority task in the task vector
	highest prioty will be 0 because task vector is sorted based on its period*/
	int curr_task = 0;
	int preempted = 0;
	for (int i = 0; i < nperiods*hperiod; ++i) {
		string *s = new string;
		*s = to_string(i);
		output_time.push(s);

		/*create task in progress*/
		task *task_ip = &task_vec[curr_task];
		preempted = 0;

		/*handles the hyperperiod case since all released at same time*/
		if (i % hperiod == 0) {
			curr_task = 0;
			task *task_ip = &task_vec[curr_task];
			task_ip-> ticks_remaining = task_ip -> wcet;

			sem_post(&sem_vec[curr_task]);
			--task_ip->ticks_remaining;
			if (task_ip-> ticks_remaining == 0) {
				++curr_task;
			}
			sem_wait(&parent);
			continue;
		}

		/*if the current task is done executing, move on until you find 
		a task that isnt finished*/
		int task_waiting = 0;
		if (task_ip-> ticks_remaining == 0) {
			for (int k = 0; k < 3; ++k) {
				curr_task = k;
				task_ip = &task_vec[curr_task];
				if (task_ip-> ticks_remaining != 0) {
					task_waiting = 1;
					break;
				}
			}
		}

		int occurences = 0;
		/*we need to iterate through every task to check if it needs to be run to meet the deadline*/
		int len = task_vec.size();
		for (int j = 0; j < len; ++j) {
			task *temp_task = &task_vec[j];
			int temp_period = temp_task->period;
			/*handles preemption case (preempt when current time equals the period of the task)*/
			if (i % temp_period == 0) {
				++occurences;

				task_ip = temp_task;
				task_ip-> ticks_remaining = task_ip-> wcet;

				/*only post semaphore for one task (highest priority),
				 even if multiple ones have the same deadline*/
				if (occurences < 2) {
					curr_task = j;

					sem_post(&sem_vec[curr_task]);
					--task_ip-> ticks_remaining;
					if (task_ip-> ticks_remaining == 0) {
						++curr_task;
					}
					sem_wait(&parent);

					preempted = 1;
					task_waiting = 1;
				}
			}
		}

		/*if a task hasn't been preempted in this time unit 
		and there's still time left for the curren task*/
		if (preempted == 0 && task_ip->ticks_remaining > 0) {
			task *task_ip = &task_vec[curr_task];
			sem_post(&sem_vec[curr_task]);
			task_ip-> ticks_remaining--;
			if (task_ip->ticks_remaining == 0) {
				++curr_task;
			}
			sem_wait(&parent);
			task_waiting = 1;
		}

		/* if no task that was executed in this tick*/
		if (task_waiting == 0) {
			string *s = new string;
			*s = "-";
			output_task.push(s);
		}
	}
	running = 0;

	/*post semaphores for each task to allow them to exit*/
	len = sem_vec.size();
	for (int i = 0; i < len; ++i)
	{
		res = sem_post(&sem_vec[i]);
		// sleep(1);
		if (res != 0) {
			cout << "error" << endl;
			return -1;
		}
	}

	/*join each thread before allowing main thread to finish*/
	len = thread_vec.size();
	for (int i = 0; i < len; ++i) {
		pthread_join(thread_vec[i],NULL);
	}

	/*destroy all semaphore states*/
	len = sem_vec.size();
	for (int i = 0; i < len; ++i) {
		sem_destroy(&sem_vec[i]);
	}
	sem_destroy(&parent);


	/*output time ticks to file*/
	while (!output_time.empty()) {
		string x = *output_time.front();
		output_time.pop();
		out_file << x;
		out_file.width(4);
	}

	out_file << '\n';
	/**/
	while (!output_task.empty()) {
		string x = *output_task.front();
		output_task.pop();
		out_file << x;
		out_file.width(4);
	}

	in_file.close();
	out_file.close();
	return 0;
}

void *func(void *param) {
	/*i dont know why, but it needs to sleep first or it hangs*/
	sleep(1);
	/*gets index from parameter, represents the task id*/
	int index = (intptr_t) param;

	sem_t *temp = &sem_vec[index];
	task *temp_task = &task_vec[index];

	sem_wait(temp);

	while (running == 1) {
		sem_post(&parent);

		output_task.push(&temp_task->id);
		
		sem_wait(temp);
	}
	pthread_exit(0);
}