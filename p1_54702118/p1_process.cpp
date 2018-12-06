#include "p1_process.h"
#include "p1_threads.h"

void get_statistics(std::string class_name[], int num_processes, int num_threads) {

	// You need to create multiple procsses threads in this file
	// Your code should be able to get the statistics and generate
	// output for any number of child processes andmak threads
	pid_t pid = 1;
	int status = 0, curr_proc;

	Context *context[MAX_PROCESS][MAX_THREADS];

	/*start process*/
	cout << "Main process is created. (pid:" << getpid() << ")"<<endl;
	for(int i = 0; i < num_processes; i++){
		if(pid > 0){
			curr_proc = i;
			pid = fork();
		}
	}

	if(pid < 0){
		fprintf(stderr, "Fork failed. (╯°□°）╯︵ ┻━┻");
		exit(-1);
	}
	// Child process 
	else if(0 == pid){
		cout << "Child process is created. (pid:" << getpid() << ")"<<endl;

		for(; curr_proc < MAX_PROCESS; curr_proc += num_processes){
			pthread_t tid[MAX_THREADS];
			/*open and parse file*/
			string inpath 	= "input/"+class_name[curr_proc]+".csv";
			string outpath	= "output/"+class_name[curr_proc]+"-stats.csv";
			
			ifstream infile (inpath.c_str());
			ofstream outfile (outpath.c_str());

			if(infile.is_open()){
				if(outfile.is_open()){
					string line;
					long int SID ;
					float grade;
					vector<pair<float, long int> > data;
					
					for(int i = 0; i < MAX_THREADS; i++){
						context[curr_proc][i] = new Context();
					}

					for(int i = 0; i < FILE_SIZE + 2; i++){
						getline(infile, line);
						const char *cline = line.c_str(); 
						char *end;

						if(i == 0){
							context[curr_proc][i]->setClassCode(cline);
						}else if(i > 1){
							SID = (long int) strtof(cline, &end);
							grade  = strtof(end,NULL);
							
							data.push_back(make_pair(grade, SID));
						}
					}
					/*sort data*/
					sort(data.begin(), data.end());

					for(int i = 0; i < FILE_SIZE; i++){
						for(int j = 0; j < MAX_THREADS; j++){
							context[curr_proc][j]->data[i] = data[i].first;
						}
					}

					/*Assign tasks to threads*/
					for(int i = 0; i < MAX_THREADS; i++){
						context[curr_proc][i % num_threads]->tasks[i] = true;
					}
					
					for(int i = 0; i < num_threads ; i++){
						if(pthread_create(&tid[i], NULL, get_stat_wrapper, 
							(void*)(context[curr_proc][i])))
						{
							fprintf(stderr, "Error creating thread. (╯°□°）╯︵ ┻━┻\n");		
						}
					}

					/* Child process must wait here until all the threads are completed
					*/
					for(int i  = 0; i < num_threads; i++){
						if(pthread_join(tid[i], NULL))
							fprintf(stderr, "Error joining thread\n");
					}

					outfile<<setprecision(5) <<context[curr_proc][0]->getClassCode() <<endl<<endl;
					for(int i = 0; i < MAX_THREADS; i++){
						outfile<< taskname[i] << ": "<< 
							context[curr_proc][i % num_threads]->result[i] << endl;
					}

					
					outfile<<endl <<"Top 3 Students:"<<endl 
						<<"Rank         Student id     Grade"<<endl;
					for(int i = 0; i < 3; i++){
						int x = 1 + i;
						for(int j = x; j < FILE_SIZE; j *= 10)
							outfile<<" ";
						outfile << x <<"      "<< data[FILE_SIZE - i - 1].second<<"     "
							 << data[FILE_SIZE - i - 1].first<<endl;
					}
					outfile<<endl <<"Middle 3 Students:"<<endl 
						<<"Rank         Student id     Grade"<<endl;
					for(int i = 0; i < 3; i++){
						int x = FILE_SIZE/2 - i + 1;
						for(int j = x; j < FILE_SIZE; j *= 10)
							cout<<" ";
						outfile << FILE_SIZE/2 + i -1<<"      "<< data[x].second<<"     "
							 << data[x].first<<endl;
					}
					outfile<<endl <<"Lowest 3 Students:"<<endl 
						<<"Rank         Student id     Grade"<<endl;
					for(int i = 2; i >= 0; i--){
						int x = FILE_SIZE - i;
						for(int j = x; j < FILE_SIZE; j *= 10)
							outfile<<" ";
						outfile << x <<"      "<< data[i].second<<"     "
							 <<data[i].first<<endl;
					}

					/*write and close file*/
					infile.close();
					outfile.close();
					/*free context*/
					for(int i = 0; i < MAX_THREADS; i++){
						delete context[curr_proc][i];
					}
				}else{
					cout<< "Failed to open file: " << outpath << endl <<" (╯°□°）╯︵ ┻━┻"<<endl;
					infile.close();		
				}
			}else{
				cout<< "Failed to open file: " << inpath << endl << " (╯°□°）╯︵ ┻━┻"<<endl;
			}
		}	
		cout << "Child process is terminated. (pid:" << getpid() << ")"<<endl;
		exit(-1);
	}
	// Parent process must wait here until all the child processes finish its behavior
	else{
		for(int i = 0; i < num_processes; i++){			
			wait(&status);
		}
	}
}