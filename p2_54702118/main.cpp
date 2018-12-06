#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "types_p2.h"
#include "p2_threads.h"
#include "utils.h"

bool finish = false;
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
struct timeval t_global_start;

int main(int argc, char** argv)
{
	// Check the argument and print error message if the argument is wrong
	if(argc != 2){
		std::cout << "[ERROR] Expecting 1 argument, but got " << argc-1 << "." << std::endl;
		std::cout << "[USAGE] p2_exec <positive number>" <<std::endl;
		return -1;
	}
	
	int number_of_ppl = 0;
	for(int i = 0; argv[1][i] != NULL; i++){
		if(!isdigit(argv[1][i])){
			std::cout << "[ERROR] Invalid argument for number of processes." << std::endl;
			std::cout << "[USAGE] p2_exec <positive number>" <<std::endl;
			return -1;
		}
		number_of_ppl = number_of_ppl*10 + argv[1][i] - 48;
	}

	/*This is to set the global start time*/
	gettimeofday(&t_global_start, NULL);

	Restroom restroom(number_of_ppl, t_global_start);

	restroom.print_status();	

	pthread_t       tid[2];
	int             status = 0;
	int             work = 0;

	/*Initialize threads*/
	if(pthread_create(&tid[0], NULL, wants_to_enter, 
			(void*) &restroom)){
		fprintf(stderr, "Error creating thread. (╯°□°）╯︵ ┻━┻\n");		
	}
	if(pthread_create(&tid[1], NULL, wants_to_leave, 
			(void*) &restroom)){
		fprintf(stderr, "Error creating thread. (╯°□°）╯︵ ┻━┻\n");		
	}

	/*Wait for threads to finish*/
	for(int i  = 0; i < 2; i++){
		if(pthread_join(tid[i], NULL))
			fprintf(stderr, "Error joining thread. (╯°□°）╯︵ ┻━┻\n");
	}

	restroom.print_status();

	return 0;
}