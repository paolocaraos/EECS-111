#include "p1_threads.h"

#include <math.h>

// This file includes functions that actually perform the
// computation. You can create a thread with these function
// or directly call this function

void* get_stat_wrapper(void *context){
	Context *con = (Context *) context;
	
	if(con->tasks[0])
		get_avg_wrapper(con);
	if(con->tasks[1])
		get_median_wrapper(con);
	if(con->tasks[2])
		get_std_dev_wrapper(con);
}

void* get_median_wrapper(Context *context) {
	/* extract data and call actual body*/
	context->result[MEDIAN] = get_median(FILE_SIZE, context->data);
}

void* get_avg_wrapper(Context *context) {
	// extract data and call actual body

	context->result[AVG] = get_avg(FILE_SIZE, context->data);
}

void* get_std_dev_wrapper(Context *context) {
	// extract data and call actual body

	context->result[STD_DEV] = get_std_dev(FILE_SIZE, context->data);
}

float get_median(int num_of_data, float* data) {
	//std::cout<<"[get_median] test data:"<< num_of_data << std::endl;

	int mid = num_of_data / 2;

	return (num_of_data % 2 == 0)? (data[mid - 1] + data[mid])/2 : data[mid];	
}

float get_avg(int num_of_data, float* data) {
	//std::cout<<"[get_avg] test data:"<< num_of_data << std::endl;

	float avg = 0;
	for(int i = 0; i < num_of_data; i++){
		avg = avg + data[i];
	}
	
	return avg / num_of_data;
}

float get_std_dev(int num_of_data, float* data) {
	//std::cout<<"[get_std_dev] test data:"<< num_of_data << std::endl;

	float sigma = 0 , diff, avg = 0;

	avg = get_avg(num_of_data, data);

	for(int i = 0; i < num_of_data; i++){
		diff = data[i] - avg;
		sigma += diff*diff;
	}

	return sqrt(sigma / (num_of_data-1));
}
