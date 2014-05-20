#ifndef stopwatch_h
#define stopwatch_h

#include <lib.core/defines.h>

typedef struct s_Stopwatch Stopwatch;

struct s_Stopwatch
{
	bool have_start;
	bool have_stop;
	struct timespec start;
	struct timespec stop;
};

bool stopwatch_start (Stopwatch *stopwatch);
bool stopwatch_stop (Stopwatch *stopwatch);
void stopwatch_reset (Stopwatch *stopwatch);
bool stopwatch_elapsed_time (Stopwatch *stopwatch, unsigned long long *milliseconds);

#endif
