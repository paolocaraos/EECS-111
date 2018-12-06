#ifndef __TYPES_P2_H
#define __TYPES_P2_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <deque>
#include <assert.h>
#include <stdexcept> 

#define MAX_SIZE 10

typedef enum{
	MALE,
	FEMALE,
	NONE
}gender_t;

class Person
{
	int gender; // 0: male 1: female
	std::string str_gender;
	bool started;
	long time_to_stay_ms;

	unsigned long order;
	unsigned long use_order;

	struct timeval t_create;
	struct timeval t_start;
	struct timeval t_end;

public:

	Person();
	Person(int g, long t_stay, unsigned long n);
	Person(struct timeval t, long stay, int gen);

	void set_gender(int data);
	int get_gender(void);

	void set_order(unsigned long data);
	unsigned long get_order(void);

	void set_use_order(unsigned long data);
	unsigned long get_use_order(void);

	void set_time(long data);
	long get_stay_time(void);
	struct timeval get_start_time(void);
	int ready_to_leave(void);

	void start(void);
	void complete(void);

	struct timeval get_time_created(void);
};

// Class for the restroom
// You may need to add more class member variables and functions
typedef enum{
	EMPTY,
	WOMENPRESENT,
	MENPRESENT
}status_t;

class Restroom {

	int status;
	bool is_full;

	int ppl_waiting[2];

	int ppl_peeing[2];

	int total_ppl;

	// You need to define the data structure to
    // save the information of people using the restroom
	// You can probebly use Standard Template Library (STL) vector
	std::deque<Person* > peeing;
	std::deque<Person> i_gotta_pee;

	struct timeval global_start_t;
public:
	
	Restroom();

	Restroom(int number_of_people, struct timeval t);

	std::deque<Person> get_ready_q();

	std::deque<Person* > get_restroom_q();

	// You need to use this function to print the Restroom's status
	void print_status(void);

	int get_status(void);
	// Call by reference
	// This is just an example. You can implement any function you need
	void add_person_into_restroom(int n);

	void woman_wants_to_enter(int n);

	void man_wants_to_enter(int n);

	void remove_person_from_restroom(int n);

	void woman_wants_to_leave(int n);

	void man_wants_to_leave(int n);

	bool isFull();
};
#endif
