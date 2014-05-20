#include <lib.core/random.h>
#include <lib.core/stopwatch.h>
#include <lib.core/error.h>
#include <stdio.h>
#include <math.h>

#include "test-random.h"

bool test_random_open_invalid_operation (Test *test)
{
	TITLE ();
	CATCH (!random_open ());
	CATCH (error_count () != 0);
	CATCH (random_open ());
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	random_close ();
	PASS ();
}

bool test_random_bytes_invalid_operation (Test *test)
{
	unsigned char bytes[1];

	TITLE ();
	CATCH (random_bytes (bytes, 1));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	PASS ();
}

bool test_random_bytes_invalid_argument_1 (Test *test)
{
	TITLE ();
	CATCH (!random_open ());
	CATCH (random_bytes (NULL, 1));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 1);
	random_close ();
	PASS ();
}

bool test_random_bytes_invalid_argument_2 (Test *test)
{
	unsigned char bytes[1];

	TITLE ();
	CATCH (!random_open ());
	CATCH (random_bytes (bytes, 0));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 2);
	random_close ();
	PASS ();
}

bool test_random_bytes (Test *test)
{
	unsigned char bytes[100];
	size_t i;
	unsigned long long sum = 0;

	TITLE ();
	for (i = 0; i < 100; i++) {
		bytes[i] = (unsigned char)0;
	}
	CATCH (!random_open ());
	CATCH (!random_bytes (bytes, 100));
	random_close ();
	for (i = 0; i < 100; i++) {
		sum += (unsigned long long)bytes[i];
	}
	CATCH (sum == 0);
	PASS ();
}

bool test_random_value (Test *test)
{
	unsigned long long value;
	unsigned int table[250];
	unsigned int n;
	unsigned int i;
	double mean;

	TITLE ();
	CATCH (!random_open ());
	CATCH (random_value (1, 0, &value));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	error_reset ();
	CATCH (!random_value (0, 0, &value));
	CATCH (value != 0);
	CATCH (!random_value ((unsigned long long)-1, (unsigned long long)-1, &value));
	CATCH (value != (unsigned long long)-1);
	for (n = 1; n < 250; n++) {
		for (i = 0; i <= n; i++) {
			table[i] = 0;
		}
		mean = 0;
		for (i = n; i > 0; i--) {
			mean += (double)n / (double)i;
		}
		while (true) {
			for (i = 0; i < (unsigned int)mean; i++) {
				CATCH (!random_value (0, n, &value));
				CATCH (value > n);
				table[value] = 1;
			}
			for (i = 0; i <= n && table[i] == 1; i++);
			if (i == n + 1)
				break;
		}
	}
	random_close ();
	PASS ();
}

bool test_random_close_invalid_operation (Test *test)
{
	TITLE ();
	random_open ();
	random_close ();
	CATCH (error_count () != 0);
	random_close ();
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	PASS ();
}
