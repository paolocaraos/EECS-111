#include "p2_threads.h"
#include "types_p2.h"
#include "utils.h"

extern bool finish;
extern pthread_cond_t  cond;
extern pthread_mutex_t mutex;

void *threadfunc(void *parm)
{
	int status;
	printf(" [Thread] Start\n");

	printf(" [Thread] Locks\n");
	status = pthread_mutex_lock(&mutex);

    printf(" [Thread] Blocked\n");
    status = pthread_cond_wait(&cond, &mutex);

	printf(" [Thread] Starts again.\n");
	for (int i=0; i<3; i++) {
		printf(" [Thread] Complete thread after (%d) seconds\n", (3-i));
		usleep(MSEC(1000));
	}

	printf(" [Thread] Unlocks\n");
	status = pthread_mutex_unlock(&mutex);
	printf(" [Thread] Complete\n");
}

void *wants_to_enter(void *parm){
	assert(parm);

	srand(time(NULL));

	Restroom *r = (Restroom *) parm;
	int status, r_status;
	int ppl[] = {0,0};
	int max = r->get_ready_q().size();
	bool ladies_turn = true;

	for(int next = 0; next < max;){

		ppl[FEMALE] = find_next(r, max, ppl[FEMALE], FEMALE);
		ppl[MALE] = find_next(r, max, ppl[MALE] , MALE);
		usleep(rand() % 5 + 1);

		status = pthread_mutex_lock(&mutex);
		if(r->isFull()){
			ladies_turn = ppl[MALE] > ppl[FEMALE];
			
			status = pthread_mutex_unlock(&mutex);
			continue;
		}
			
		if(ladies_turn && ppl[FEMALE] < max && r->get_status() != MENPRESENT){
			r->add_person_into_restroom(ppl[FEMALE]++);
			/*increment f or m after adding*/
		}else if(!ladies_turn && ppl[MALE] < max && r->get_status() != WOMENPRESENT){
			r->add_person_into_restroom(ppl[MALE]++);
		}
		status = pthread_mutex_unlock(&mutex);

		if(ppl[MALE] >= max)
			ladies_turn = true;
		if(ppl[FEMALE] >= max)
			ladies_turn = false;

		next = (ppl[MALE] > ppl[FEMALE])? ppl[FEMALE] : ppl[MALE];
	}

	status = pthread_mutex_lock(&mutex);
	r->print_status();
	std::cout<<"Everybody has gone." <<std::endl;
	finish = true;
	status = pthread_mutex_unlock(&mutex);

	return 0;
}

void *wants_to_leave(void *parm){
	assert(parm);

	Restroom *r = (Restroom *) parm;
	int status, i = 0; 
	bool isEmpty = false, done = false;
	
	while(!finish || !isEmpty){
		usleep(1);
		status = pthread_mutex_lock(&mutex);
		if(r->get_status() == EMPTY){
			isEmpty = true;
			status = pthread_mutex_unlock(&mutex);
			usleep(6);
			continue;
		}else{
			isEmpty =false;
		}
		try{
			if(1 == r->get_restroom_q().at(i)->ready_to_leave()){
				r->remove_person_from_restroom(i);
			}	
		}catch(const std::out_of_range& oor){
			//std::cout<<"There's no one in this stall." <<std::endl;
		}
		status = pthread_mutex_unlock(&mutex);
		//r->print_status();
		i = (i +1) % MAX_SIZE;
	}
	return 0 ;
}

int find_next(Restroom *r, int max, int next, int gender){
	for(; next < max; next++ ){
		try{
			if(r->get_ready_q().at(next).get_gender() == gender){
				break;
			}
		}catch(const std::out_of_range& oor){

		}
	}
	return next;
}