#ifndef test_h 
#define test_h

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct _Test Test;

struct _Test
{
	const char *title;
	size_t line_catch;
	size_t ms;
};

bool test_array (int argc, char **argv, bool (*tests[]) (Test *test));

#define TITLE() \
	test->title = __FUNCTION__;
#define CATCH(condition) \
	if (condition) { \
		test->line_catch = __LINE__; \
		return false; \
	}
#define CATCH_CLEAN(condition,clean) \
	if (condition) { \
		test->line_catch = __LINE__; \
		clean; \
		return false; \
	}
#define PASS() \
	return true;

#endif
