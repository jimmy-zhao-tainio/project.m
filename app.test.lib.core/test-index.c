#include <lib.core/index.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>

#include "test-index.h"

bool test_index_create_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (index_create (0));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_index_create_overflow_1 (Test *test)
{
	TITLE ();
	CATCH (index_create (SIZE_MAX));
	CATCH (error_at (0).error != ErrorOverflow);
        CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_index_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_total_create_limit (368 - 1);
        CATCH (index_create (1));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_index_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_total_create_limit (384 - 1);
        CATCH (index_create (1));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_index_create_function_call_3 (Test *test)
{
        TITLE ();
        memory_total_create_limit (400 - 1);
        CATCH (index_create (1));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_index_create_function_call_4 (Test *test)
{
        TITLE ();
        memory_total_create_limit (409 - 1);
        CATCH (index_create (1));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 4);
        PASS ();
}

bool test_index_create (Test *test)
{
        Index *index;

        TITLE ();
        CATCH (!(index = index_create (1)));

        index_destroy (index);
        PASS ();
}
