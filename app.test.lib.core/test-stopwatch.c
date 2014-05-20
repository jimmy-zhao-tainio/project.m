#include <lib.core/stopwatch.h>
#include <lib.core/error.h>

#include "test-stopwatch.h"

bool test_stopwatch_start_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (stopwatch_start (NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_stopwatch_start_invalid_operation (Test *test)
{
	Stopwatch stopwatch;

	TITLE ();
	stopwatch_reset (&stopwatch);
	CATCH (!stopwatch_start (&stopwatch));
	CATCH (stopwatch_start (&stopwatch));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	PASS ();
}

bool test_stopwatch_stop_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (stopwatch_stop (NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_stopwatch_stop_invalid_operation_1 (Test *test)
{
	Stopwatch stopwatch;

	TITLE ();
	stopwatch_reset (&stopwatch);
	CATCH (stopwatch_stop (&stopwatch));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_stopwatch_stop_invalid_operation_2 (Test *test)
{
	Stopwatch stopwatch;

	TITLE ();
	stopwatch_reset (&stopwatch);
	CATCH (!stopwatch_start (&stopwatch));
	CATCH (!stopwatch_stop (&stopwatch));
	CATCH (stopwatch_stop (&stopwatch));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (error_at (0).code != 2);
	PASS ();
}

bool test_stopwatch_reset_invalid_argument (Test *test)
{
	TITLE ();
	stopwatch_reset (NULL);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_stopwatch_reset (Test *test)
{
	Stopwatch stopwatch;

	TITLE ();
	stopwatch.have_start = true;
	stopwatch.have_stop = true;
	stopwatch_reset (&stopwatch);
	CATCH (stopwatch.have_start != false);
	CATCH (stopwatch.have_stop != false);
	PASS ();
}

bool test_stopwatch_elapsed_time_invalid_argument_1 (Test *test)
{
	TITLE ();
	CATCH (stopwatch_elapsed_time (NULL, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_stopwatch_elapsed_time_invalid_argument_2 (Test *test)
{
	Stopwatch stopwatch;

	TITLE ();
	CATCH (stopwatch_elapsed_time (&stopwatch, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 2);
	PASS ();
}

bool test_stopwatch_elapsed_time_invalid_operation_1 (Test *test)
{
	Stopwatch stopwatch;
	unsigned long long ms;

	TITLE ();
	stopwatch_reset (&stopwatch);
	CATCH (stopwatch_elapsed_time (&stopwatch, &ms));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_stopwatch_elapsed_time_invalid_operation_2 (Test *test)
{
	Stopwatch stopwatch;
	unsigned long long ms;

	TITLE ();
	stopwatch_reset (&stopwatch);
	CATCH (!stopwatch_start (&stopwatch));
	CATCH (stopwatch_elapsed_time (&stopwatch, &ms));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (error_at (0).code != 2);
	PASS ();
}

bool test_stopwatch_elapsed_time_invalid_operation_3 (Test *test)
{
	Stopwatch stopwatch;
	unsigned long long ms;

	TITLE ();
	stopwatch_reset (&stopwatch);
	CATCH (!stopwatch_start (&stopwatch));
	CATCH (!stopwatch_stop (&stopwatch));
	stopwatch.start.tv_sec = 1;
	stopwatch.start.tv_nsec = 0;
	stopwatch.stop.tv_sec = 0;
	stopwatch.stop.tv_nsec = 0;
	CATCH (stopwatch_elapsed_time (&stopwatch, &ms));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (error_at (0).code != 3);
	PASS ();
}

bool test_stopwatch_elapsed_time (Test *test)
{
	Stopwatch stopwatch;
	unsigned long long ms;

	TITLE ();
	stopwatch_reset (&stopwatch);
	CATCH (!stopwatch_start (&stopwatch));
	CATCH (!stopwatch_stop (&stopwatch));
	CATCH (!stopwatch_elapsed_time (&stopwatch, &ms));
	CATCH (ms > 1000);
	PASS ();
}
