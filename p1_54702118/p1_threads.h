#ifndef __P1_THREADS
#define __P1_THREADS

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <functional>
#include <algorithm>
#include <vector>
#include <utility>
#include <iomanip>
#include <assert.h>
#include "p1_process.h"

using namespace std;

#define MAX_THREADS 	3
#define FILE_SIZE 		1000000

typedef enum{
	AVG,
	MEDIAN,
	STD_DEV
}task;

const string taskname[] = {
	"Average",
	"Median",
	"Std. Dev"
};

class Context{
	string class_code;

public:
	float result[MAX_THREADS];
	bool tasks[MAX_THREADS];
	float data[FILE_SIZE];
	Context()
	{	
		for(int i = 0; i < MAX_THREADS; i ++){
			result[i] = 0;
			tasks[i] = false;
		}
	}

	string getClassCode(){
		return class_code;
	}

	void setClassCode(string x){
		class_code = x;
	}
};

void* get_median_wrapper(Context *context);
void* get_avg_wrapper(Context *context);
void* get_std_dev_wrapper(Context *context);

void* get_stat_wrapper(void *context);

/*
typedef void(*get_stat_t)(Context *)
get_stat_t get_stat[] = {
	get_median_wrapper,
	get_avg_wrapper,
	get_std_dev_wrapper
};*/

float get_median(int num_of_data, float* data);
float get_avg(int num_of_data, float* data);
float get_std_dev(int num_of_data, float* data);

#endif
