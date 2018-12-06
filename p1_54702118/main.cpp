#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <pthread.h>

#include "p1_process.h"
#include "p1_threads.h"

int main(int argc, char** argv)
{
	std::string class_name[] = {"os", "architecture", "java", "algorithm", "digital-design"};
	std::cout << "Beginning of the main()" << std::endl;
	// Check the argument and print error message if the argument is wrong

	if(argc != 3){
		std::cout << "[ERROR] Expecting 2 arguments, but got " << argc-1 << "." << std::endl;
		std::cout << "[USAGE] p1_exec <number of processes> <number of threads>" <<std::endl;
		std::cout << "where <number of processes> is a number between 1 and 5" <<std::endl;
		return -1;
	}
	if(!isdigit(argv[1][0]) || argv[1][1] != NULL){
		std::cout << "[ERROR] Invalid argument for number of processes." << std::endl;
		std::cout << "[USAGE] p1_exec <number of processes> <number of threads>" <<std::endl;
		std::cout << "where <number of processes> is a number between 1 and 5" <<std::endl;
		return -1;
	}
	if(!isdigit(argv[2][0]) || argv[2][1] != NULL){
		std::cout << "[ERROR] Invalid argument for number of threads." << std::endl;
		std::cout << "[USAGE] p1_exec <number of processes> <number of threads>" <<std::endl;
		std::cout << "where <number of threads> is a number between 1 and 3" <<std::endl;
		return -1;
	}

	int num_processes = argv[1][0] - 48;
	int num_threads = argv[2][0] - 48;

	if(num_threads > 3 || num_threads < 1){
		std::cout << "[ERROR] Invalid number of threads." << std::endl;
		std::cout << "[USAGE] p1_exec <number of processes> <number of threads>" <<std::endl;
		std::cout << "where <number of threads> is a number between 1 and 3" <<std::endl;
		return -1;
	}

	if(num_processes > 5 || num_processes < 1){
		std::cout << "[ERROR] Invalid number of processes." << std::endl;
		std::cout << "[USAGE] p1_exec <number of processes> <number of threads>" <<std::endl;
		std::cout << "where <number of processes> is a number between 1 and 5" <<std::endl;
		return -1;
	}

	// Call the process
	get_statistics(class_name, num_processes, num_threads);

	std::cout << "End of the main()" << std::endl;
	return 0;	
}

