#include <stdlib.h>
#include <stdio.h>

#include <lib.core/error.h>
#include <lib.core/queue.h>
#include <lib.core/queue-private.h>
#include <lib.core/memory.h>
#include <lib.core/string.h>
#include <lib.core/defines-private.h>

#include "test-queue.h"

bool test_queue_create_function_call_1 (Test *test)
{
	Queue queue;

	TITLE ();
	CATCH (queue_create (&queue, (size_t)-1, (size_t)-1, QUEUE_SIZE_FIXED));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_queue_create_invalid_operation (Test *test)
{
	Queue queue;

	TITLE ();
	CATCH (queue_create (&queue, 0, 0, QUEUE_SIZE_FIXED));
        CATCH (error_at (0).error != ErrorInvalidOperation);
	PASS ();
}

bool test_queue_create_function_call_2 (Test *test)
{
	Queue queue;

	TITLE ();
	CATCH (queue_create (&queue, 1, (size_t)-1, QUEUE_SIZE_FIXED));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
	PASS ();
}

bool test_queue_create (Test *test)
{
        Queue queue;
        
        TITLE ();
	CATCH (!queue_create (&queue, 3, 5, QUEUE_SIZE_FIXED));
        queue_destroy (&queue);
	PASS ();
}

bool test_queue_destroy_invalid_operation (Test *test)
{
	Queue queue = { .items = NULL };

	TITLE ();
	queue_destroy (&queue);
        CATCH (error_at (0).error != ErrorInvalidOperation);
	PASS ();
}

bool test_queue_push_invalid_operation_1 (Test *test)
{
        Queue queue;
        struct item { char c; };
        struct item a;
        
        TITLE ();
        CATCH (!queue_create (&queue, 3, sizeof (struct item), QUEUE_SIZE_FIXED));
        CATCH (!queue_push (&queue, &a));
        CATCH (!queue_push (&queue, &a));
        CATCH (!queue_push (&queue, &a));
        CATCH (queue_push (&queue, &a));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 1);
        queue_destroy (&queue);
        PASS ();
}

bool test_queue_push_invalid_operation_2 (Test *test)
{
        Queue queue;
        struct item { char c; };
        struct item a;
        
        TITLE ();
        CATCH (!queue_create (&queue, 3, sizeof (struct item), QUEUE_SIZE_DYNAMIC));
        CATCH (!queue_push (&queue, &a));
        CATCH (!queue_push (&queue, &a));
        CATCH (!queue_push (&queue, &a));
        memory_commit_limit (0);
        CATCH (queue_push (&queue, &a));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 2);
        CATCH (error_at (1).error != ErrorFunctionCall);
        queue_destroy (&queue);
        PASS ();
}

bool test_queue_grow_invalid_operation (Test *test)
{
        Queue queue;
        
        TITLE ();
        CATCH (!queue_create (&queue, 1, sizeof (char), QUEUE_SIZE_DYNAMIC));
        queue.item_capacity = (size_t)-1;
        CATCH (queue_grow (&queue));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        queue_destroy (&queue);
        PASS ();
}

bool test_queue_grow_function_call (Test *test)
{
        Queue queue;
        
        TITLE ();
        CATCH (!queue_create (&queue, 1, sizeof (char), QUEUE_SIZE_DYNAMIC));
        memory_commit_limit (0);
        CATCH (queue_grow (&queue));
        CATCH (error_at (0).error != ErrorFunctionCall);
        queue_destroy (&queue);
        PASS ();
}
