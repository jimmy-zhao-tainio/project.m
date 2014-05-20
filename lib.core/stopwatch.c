#include <lib.core/stopwatch.h>
#include <lib.core/error.h>
#include <time.h>

bool stopwatch_start (Stopwatch *stopwatch)
{
	if (!stopwatch) {
		error (InvalidArgument);
		return false;
	}
	if (stopwatch->have_start == true) {
		error (InvalidOperation);
		return false;
	}
	if (clock_gettime (CLOCK_MONOTONIC, &stopwatch->start) != 0) {
		error (SystemCall);
		return false;
	}
	stopwatch->have_start = true;
	return true;
}

bool stopwatch_stop (Stopwatch *stopwatch)
{
	if (!stopwatch) {
		error (InvalidArgument);
		return false;
	}
	if (stopwatch->have_start == false) {
		error_code (InvalidOperation, 1);
		return false;
	}
	if (stopwatch->have_stop == true) {
		error_code (InvalidOperation, 2);
		return false;
	}
	if (clock_gettime (CLOCK_MONOTONIC, &stopwatch->stop) != 0) {
		error (SystemCall);
		return false;
	}
	stopwatch->have_stop = true;
	return true;
}

void stopwatch_reset (Stopwatch *stopwatch)
{
	if (!stopwatch) {
		error (InvalidArgument);
		return;
	}
	stopwatch->have_start = false;
	stopwatch->have_stop = false;
}

bool stopwatch_elapsed_time (Stopwatch *stopwatch, unsigned long long *milliseconds)
{
	unsigned long long stop_ms;
	unsigned long long start_ms;

	if (!stopwatch) {
		error_code (InvalidArgument, 1);
		return false;
	}
	if (!milliseconds) {
		error_code (InvalidArgument, 2);
		return false;
	}
	if (stopwatch->have_start == false) {
		error_code (InvalidOperation, 1);
		return false;
	}
	if (stopwatch->have_stop == false) {
		error_code (InvalidOperation, 2);
		return false;
	}
	stop_ms = (unsigned long long)stopwatch->stop.tv_sec * 1000;
	stop_ms = ((unsigned long long)stopwatch->stop.tv_nsec / 1000000) + stop_ms;
	start_ms = (unsigned long long)stopwatch->start.tv_sec * 1000;
	start_ms = ((unsigned long long)stopwatch->start.tv_nsec / 1000000) + start_ms;
	if (stop_ms < start_ms) {
		error_code (InvalidOperation, 3);
		return false;
	}
	*milliseconds = stop_ms - start_ms;
	return true; 
}
