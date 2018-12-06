#ifndef __P2_THREADS_H
#define __P2_THREADS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <vector>
#include <unistd.h>
#include "types_p2.h"
#include <assert.h>

void *threadfunc(void *parm);

void *wants_to_enter(void *parm);

void *wants_to_leave(void *parm);

int find_next(Restroom *r, int max, int next, int gender);

#endif