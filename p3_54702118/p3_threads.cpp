#include "p3_threads.h"
#include "utils.h"

extern pthread_mutex_t mutex;
extern pthread_mutex_t exec;
extern pthread_cond_t  cond[4];
extern std::vector<int> readyQue;
extern int occupied;

extern int global_work; // DO NOT INITIALIZE!!

void *threadfunc(void *param)
{

	long stamp;
	ThreadCtrlBlk* tcb = (ThreadCtrlBlk*)param;	
	int iter = 1;
	int id = tcb->id;
	int sleep = 0;
	int fail = 0;

	printf(" - [Thread %d] Start\n", id);

	while (true) {
		pthread_mutex_lock(&mutex);
		if (global_work==0) {
			pthread_mutex_unlock(&mutex);
			break;
		}
		// push id
		readyQue.push_back(id);
		pthread_mutex_unlock(&mutex);


		// goes into sleep
		stamp = get_time_stamp();
		printf("[%lu ms][Thread %d] Goto sleep\n", stamp, id);
		pthread_cond_wait(&(cond[id]), &exec);
		if (global_work==0) { pthread_mutex_unlock(&exec); break; }

		// unlock & block
		stamp = get_time_stamp();
		printf("[%lu ms][Thread %d] Start Task (%d, %d) (%lu)ms\n", stamp, id, id,iter, tcb->task_time);
		// perform task
		occupied = 1;
		usleep(MSEC(tcb->task_time));
		stamp = get_time_stamp();
		printf("[%lu ms][Thread %d] Complete Task (%d, %d) (%lu)ms\n", stamp, id, id,iter, tcb->task_time);

		if (stamp > (tcb->deadline + 50)) {
			printf("[%lu ms][Thread %d] Task (%d, %d) MISS the DEADLINE!!!! (Deadline: %lu)\n", stamp, id, id,iter, tcb->deadline);
			fail = 1;
			occupied = 0;
			pthread_mutex_unlock(&exec);
			break;
		}
		occupied = 0;
		pthread_mutex_unlock(&exec);

		// estimate waiting time
		stamp = get_time_stamp();
		sleep = iter * tcb->period - stamp;
		iter++;

		// set deadline for next task
		pthread_mutex_lock(&mutex);
		tcb->deadline = iter * tcb->period;
		pthread_mutex_unlock(&mutex);

		usleep(MSEC(sleep));

	}


	printf(" - [Thread %d] Complete\n", id);
	if (fail) {
		printf(" - [Thread %d] MISS the DEADLINE\n", id);
	}
}
