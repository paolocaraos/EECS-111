#include "types_p2.h"
#include "utils.h"

Person::Person(){
	gettimeofday(&t_create, NULL);
	started = false;
}

Person::Person(int g, long t_stay, unsigned long n) : gender(g),  order(n){
	gettimeofday(&t_create, NULL);
	time_to_stay_ms = t_stay;
	started = false;
}

Person::Person(struct timeval t, long stay, int gen){
	t_create = t;
	time_to_stay_ms = stay;
	gender = gen;	
	started = false;
}

void Person::set_gender(int data) { gender = data; }

int Person::get_gender(void)      { return gender; }

void Person::set_order(unsigned long data) { order = data; }

unsigned long Person::get_order(void)      { return order; }

void Person::set_use_order(unsigned long data) { use_order = data; }

unsigned long Person::get_use_order(void)      { return use_order; }

void Person::set_time(long data) { time_to_stay_ms = data; }

long Person::get_stay_time(void) {return time_to_stay_ms;}

struct timeval Person::get_start_time(void) {return t_start;}

int Person::ready_to_leave(void) {
	struct timeval t_curr;
	gettimeofday(&t_curr, NULL);

	if (get_elasped_time(t_start, t_curr) >= time_to_stay_ms) { return 1; }
	else { 
		//std::cout<<"Not yet, "<< get_elasped_time(t_start, t_curr)<< " more milliseconds" <<std::endl;
		return 0; 
	}
}

void Person::start(void) {
	started = true;
	gettimeofday(&t_start, NULL);
	printf("(%lu)th person enters the restroom: \n", order);
	printf(" - (%lu) milliseconds after the creation\n", get_elasped_time(t_create, t_start));
}

void Person::complete(void) {
	gettimeofday(&t_end, NULL);
	printf("(%lu)th person comes out of the restroom: \n", order);
	printf(" - (%lu) milliseconds after the creation\n", get_elasped_time(t_create, t_end));
	printf(" - (%lu) milliseconds after using the restroom\n", get_elasped_time(t_start, t_end));
}

struct timeval Person::get_time_created(void){
	return t_create;
}

Restroom::Restroom(){
	status = EMPTY;
	is_full = false;
}

Restroom::Restroom(int number_of_ppl, struct timeval t){

	srand(time(NULL));

	struct timeval current_time;
	status = EMPTY;
	global_start_t = t;
	ppl_peeing[MALE] = ppl_peeing[FEMALE] = 0;

	int number_per_gender[2];
	ppl_waiting[MALE] = ppl_waiting[FEMALE]
		= number_per_gender[MALE] = number_per_gender[FEMALE] = number_of_ppl; 
	for(int i = 0; 0 < number_per_gender[MALE] + number_per_gender[FEMALE]; i++){

		int x = rand();
		int r = x % 2;
		long t_stay = (long) (x % 8) + 3;
		int other;

		if(number_per_gender[r] > 0){
			i_gotta_pee.push_back(Person(r, t_stay, i+1));
			number_per_gender[r]--;
		}else if(number_per_gender[other = (r+1) % 2] > 0){
			i_gotta_pee.push_back(Person(other, t_stay, i+1));
			number_per_gender[other]--;
		}

		gettimeofday(&current_time, NULL);
		std::cout<< "[" << get_elasped_time(global_start_t, current_time) 
				 << " ms ] [Input] A person goes into the queue." <<std::endl;
	}

	std::cout<<"Restroom ready queue:"<<std::endl;
	for(int i = 0; i < number_of_ppl*2; i++){
		(i_gotta_pee[i].get_gender() == MALE)?
			std::cout <<i+1 <<" : MALE   ":
			std::cout <<i+1 <<" : FEMALE ";
		std::cout<<" PEE TIME: "<< i_gotta_pee[i].get_stay_time() <<" ms "<<std::endl;
	}
}

// You need to use this function to print the Restroom's status
void Restroom::print_status(void) {
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	std::cout<<"Print restroom status"<<std::endl;
	switch(status){
		case EMPTY:
			std::cout<<"["<< get_elasped_time(global_start_t, current_time) 
					<<" ms ] [Restroom] State: EMPTY, ";
			break;
		case WOMENPRESENT:
			std::cout<<"["<< get_elasped_time(global_start_t, current_time) 
					<<" ms ] [Restroom] State: WOMENPRESENT (Number: "
					<<ppl_peeing[FEMALE]<<"), ";
			break;
		case MENPRESENT:
			std::cout<<"["<< get_elasped_time(global_start_t, current_time) 
					<<" ms ] [Restroom] State: MENPRESENT (Number: "
					<<ppl_peeing[MALE]<<"), ";
			break;
		default:
			std::cout<<"¯\_(ツ)_/¯"<<std::endl;
			break;
	}
	std::cout<<"Queue status: Total: "<< ppl_waiting[MALE] + ppl_waiting[FEMALE] 
			 <<"(Men: "<< ppl_waiting[MALE] <<", Women: "<<ppl_waiting[FEMALE]<< ")"<<std::endl; 
}

int Restroom::get_status(){
	if(ppl_peeing[MALE] > 0)
		status = MENPRESENT;
	else if( ppl_peeing[FEMALE] > 0)
		status = WOMENPRESENT;
	else{
		status = EMPTY;
	}

	return status;
}

// Call by reference
// This is just an example. You can implement any function you need
void Restroom::add_person_into_restroom(int n) {
	Person p = i_gotta_pee.at(n);
	int gen = p.get_gender(), x;
	struct timeval current_time;
	gettimeofday(&current_time, NULL);

	ppl_waiting[gen]--;
	ppl_peeing[gen]++;

	for(int i  = 0 ; i < MAX_SIZE; i++){
		try{
			peeing.at(i);
		}catch(const std::out_of_range& oor) {
			peeing.insert(peeing.begin() + i,  
				&i_gotta_pee[n]);
			x = i;
			break;
		}
	}

	peeing.at(x)->start();

	if(gen == MALE){
		std::cout<<"[" << get_elasped_time(global_start_t, current_time)
				 <<"ms ] A man goes into the restroom. (Stay "<< p.get_stay_time() << " ms )"
				 <<std::endl;
		status = MENPRESENT;
	}else{
		std::cout<<"[" << get_elasped_time(global_start_t, current_time)
				 <<"ms ] A woman goes into the restroom. (Stay "<< p.get_stay_time() << " ms )"
				 <<std::endl;
		status = WOMENPRESENT;
	}
}

void Restroom::remove_person_from_restroom(int n){
	int gen = peeing.at(n)->get_gender();

	(gen == MALE)? man_wants_to_leave(n) : woman_wants_to_leave(n);
}

void Restroom::woman_wants_to_leave(int n){
	peeing[n]->complete();
	peeing.erase(peeing.begin() +n);
	status = (--ppl_peeing[FEMALE] == 0)? EMPTY : WOMENPRESENT;
}

void Restroom::man_wants_to_leave(int n){
	peeing[n]->complete();
	peeing.erase(peeing.begin() +n);
	status = (--ppl_peeing[MALE] == 0)? EMPTY : MENPRESENT;
}

bool Restroom::isFull(){
	return is_full = (ppl_peeing[MALE] >= MAX_SIZE || ppl_peeing[FEMALE] >= MAX_SIZE);
}

std::deque<Person> Restroom::get_ready_q(){
	return i_gotta_pee;
}

std::deque<Person*> Restroom::get_restroom_q(){
	return peeing;
}