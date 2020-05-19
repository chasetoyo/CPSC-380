# Pseudo Rate Monotonic Scheduler

## Chase Toyofuku-Souza
> 2296478

> toyofukusouza@chapman.edu

----
### About
A C++ program that simulates a Rate Monotonic (RM) real-scheduler. The scheduler will create n number of threads (based upon the tasks defined in the task set file). Then simulate the scheduling of those threads using posix based semaphores. Each thread will wait in a while loop waiting to acquire the semaphore. Once acquired the thread will print-out just its task name then go wait for the next semaphore post by the scheduler (Only one function should be needed to implement the thread tasks). The scheduler will release or post a semaphore (so n tasks means n sempahores) based upon the RM scheduling algorithm. A “clock tick” will be simulated through each iteration of the main scheduling loop (i.e. one iteration first clock tick, two iterations, second clock tick,). Assume all task are periodic and released at time 0.

### Files
- rmsched.cc
- task_set.txt

### Instructions
- g++ -pthread rmsched.cc -o rmsched
- ./rmsched <n periods> <task set> <schedule>

### Notes
The format of the task set schedule is 
task id,wcet,period
task id2,wcet2,period2
.
.
.