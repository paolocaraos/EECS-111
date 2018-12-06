#include "utils.h"

extern struct timeval t_global_start;

long get_elasped_time(struct timeval& start, struct timeval& end)
{

	long mtime = 0;
    long seconds  = end.tv_sec  - start.tv_sec;
    long useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

	return mtime;
}


long get_time_stamp(void)
{
	struct timeval t_curr;
	long mtime = 0;

	gettimeofday(&t_curr, NULL);
    long seconds  = t_curr.tv_sec  - t_global_start.tv_sec;
    long useconds = t_curr.tv_usec - t_global_start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

	return mtime;
}
