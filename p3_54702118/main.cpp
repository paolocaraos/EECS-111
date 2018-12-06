#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include "types_p3.h"
#include "p3_threads.h"
#include "utils.h"
#include <stdexcept>

pthread_cond_t  cond[4];
ThreadCtrlBlk   tcb[4];
std::vector<int> readyQue;
int occupied = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t exec = PTHREAD_MUTEX_INITIALIZER;
struct timeval t_global_start;
int global_work = 0;

void fifo_schedule(void);
void edf_schedule(void);
void rm_schedule(void);

int main(int argc, char** argv)
{
	// Check the argument and print error message if the argument is wrong
	if(argc != 2){
		std::cout << "[ERROR] Expecting 1 argument, but got " << argc-1 << "." << std::endl
				  << "[USAGE] p2_exec <positive number>" <<std::endl;
		return -1;
	}
	
	int sch = 0;
	for(int i = 0; argv[1][i] != NULL; i++){
		if(!isdigit(argv[1][i])){
			std::cout<< "[ERROR] Invalid argument for scheduling specifier." << std::endl
					 << "[USAGE] p2_exec <positive number>" <<std::endl
					 << "where <positive number> is " <<std::endl
					 << "0: for FIFO scheduling " <<std::endl
					 << "1: for EDF scheduling " <<std::endl
					 << "2: for RM scheduling " <<std::endl;
			return -1;
		}
		sch = sch*10 + argv[1][i] - 48;
	}

	if(sch > 3){
		std::cout << "[ERROR] Invalid argument for scheduling specifier." << std::endl
				  << "[USAGE] p2_exec <positive number>" <<std::endl
					 << "where <positive number> is " <<std::endl
					 << "0: for FIFO scheduling " <<std::endl
					 << "1: for EDF scheduling " <<std::endl
					 << "2: for RM scheduling " <<std::endl;
		return -1;
	}

	pthread_t       tid[4];
	int             status = 0;
	int             id = 0;
	long            stamp = 0;
	int             mode = 0;

	// Get the mode information
	tcb[0].id = 0;
	tcb[0].task_time = 200;
	tcb[0].period = 1000;
	tcb[0].deadline = 1000;

	tcb[1].id = 1;
	tcb[1].task_time = 500;
	tcb[1].period = 2000;
	tcb[1].deadline = 2000;

	tcb[2].id = 2;
	tcb[2].task_time = 1000;
	tcb[2].period = 4000;
	tcb[2].deadline = 4000;

	tcb[3].id = 3;
	tcb[3].task_time = 1000;
	tcb[3].period = 6000;
	tcb[3].deadline = 6000;

	for (int i=0; i<4; i++) {
		pthread_cond_init (&(cond[i]), NULL); 
	}

	switch(sch){
		case 0:
			std::cout<<"FIFO schedule."<<std::endl;
			break;
		case 1:
			std::cout<<"EDF schedule."<<std::endl;
			break;
		case 2:
			std::cout<<"Not implemented."<<std::endl;
			break;
		default:
			std::cout<<"Invalid input."<<std::endl;
			return 0;
	}

	// This is to set the global start time
	gettimeofday(&t_global_start, NULL);
	global_work = 1;
	printf("[Main] Create worker threads\n");
	for (int i=0; i<4; i++) {
		if(pthread_create(&(tid[i]), NULL, threadfunc, &(tcb[i]))) {
			fprintf(stderr, "Error creating thread\n");		
		}
		usleep(MSEC(1));
	}

	// Wait until the thread is in place
	usleep(MSEC(5));
	int sleep = 0;
	for (int i=0; i<2400; i++) {	
		stamp = get_time_stamp();
		//printf("[%lu ms] Loop starts\n", stamp);

		/////////////////////////////////////////////////////////
		// Implement different scheduling algorithms
        // Select different schedule based on mode value
		switch(sch){
			case 0:
				fifo_schedule();
				break;
			case 1:
				edf_schedule();
				break;
			case 2:
				break;
			default:
				return 0;
		}
		/////////////////////////////////////////////////////////

		stamp = get_time_stamp();
		sleep = 10 - (stamp%10);
		
		//printf("[%lu ms] Sleep (%d)ms\n", stamp, sleep);

		usleep(MSEC(sleep));
	}

	printf("[Main] It's time to finish the thread\n");
	printf("[Main] Locks\n");
	pthread_mutex_lock(&mutex);
	global_work = 0;
	printf("[Main] Unlocks\n");
	pthread_mutex_unlock(&mutex);

	while (readyQue.size()>0) {
		pthread_cond_signal(&(cond[readyQue[0]]));
		readyQue.erase(readyQue.begin()+0);
	}

	/* wait for the second thread to finish */
	for (int i=0; i<4; i++) {
		if(pthread_join(tid[i], NULL)) {
			fprintf(stderr, "Error joining thread\n");	
		}
	}
	return 0;
}

void fifo_schedule(void)
{
	static int curr_thread;
	static long finish_t = 0, current_time;

	if((current_time = get_time_stamp()) % 1000 == 0)
		usleep(500);

	pthread_mutex_lock(&mutex);
	if(readyQue.size() == 0 || get_time_stamp() < finish_t){
		pthread_mutex_unlock(&mutex);
		return  ;
	}
	curr_thread = readyQue[0];
	readyQue.erase(readyQue.begin()+0);
	pthread_cond_signal(&(cond[curr_thread]));
	pthread_mutex_unlock(&mutex);
	finish_t =  get_time_stamp() + tcb[curr_thread].task_time;
}

void edf_schedule(void)
{
	static int curr_thread;
	static long finish_t = 0, current_time;
	int size, k;
	long min_d = 2147483648, min_t = 2147483648, curr_d, curr_t;

	if((current_time = get_time_stamp()) % 1000 == 0)
		usleep(500);

	pthread_mutex_lock(&mutex);
	if((size = readyQue.size()) == 0 || get_time_stamp() < finish_t){
		pthread_mutex_unlock(&mutex);
		return  ;
	}

	if(size == 1 && readyQue[0] == 3 && (get_time_stamp() % 12000) <= 2000){
		pthread_mutex_unlock(&mutex);
		return;
	}

	//std::cout<<"["<<get_time_stamp()<<" ms][Main] Queue: ";
	for(int i = 0; i < size ; i++){
		//std::cout<<readyQue[i]<<" ";
		curr_d = tcb[readyQue[i]].deadline;
		curr_t = tcb[readyQue[i]].task_time;
		if(min_d > curr_d || (min_d == curr_d && min_t > curr_t)){
			min_d = curr_d;
			min_t = curr_t;
			curr_thread = readyQue[i];
			k = i;
		}
	}
	//std::cout<<std::endl;

	readyQue.erase(readyQue.begin()+k);
	pthread_cond_signal(&(cond[curr_thread]));
	pthread_mutex_unlock(&mutex);
	finish_t =  get_time_stamp() + min_t;
}

void rm_schedule(void)
{
	;
}








