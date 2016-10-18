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

bool test_queue_grow_invalid_operation_1 (Test *test)
{
        Queue queue;
        
        TITLE ();
        CATCH (!queue_create (&queue, 1, sizeof (char), QUEUE_SIZE_DYNAMIC));
        CATCH (queue_grow (&queue));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 1);
        queue_destroy (&queue);
        PASS ();
}

bool test_queue_grow_invalid_operation_2 (Test *test)
{
        Queue queue;
        
        TITLE ();
        CATCH (!queue_create (&queue, 1, sizeof (char), QUEUE_SIZE_DYNAMIC));
        queue.item_capacity = (size_t)-1;
        queue.item_count = (size_t)-1;
        CATCH (queue_grow (&queue));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 2);
        queue_destroy (&queue);
        PASS ();
}

bool test_queue_grow_function_call (Test *test)
{
        Queue queue;
        char a;
        
        TITLE ();
        CATCH (!queue_create (&queue, 1, sizeof (char), QUEUE_SIZE_DYNAMIC));
        CATCH (!queue_push (&queue, &a));
        memory_commit_limit (0);
        CATCH (queue_grow (&queue));
        CATCH (error_at (0).error != ErrorFunctionCall);
        queue_destroy (&queue);
        PASS ();
}

bool test_queue_grow_1 (Test *test)
{
        Queue queue;
        char a;

        TITLE ();
        CATCH (!queue_create (&queue, 1, sizeof (char), QUEUE_SIZE_DYNAMIC));
        CATCH (!queue_push (&queue, &a));
        CATCH (!queue_push (&queue, &a));
        CATCH (queue.item_capacity != 2);
        CATCH (!queue_push (&queue, &a));
        CATCH (queue.item_capacity != 3);
        CATCH (!queue_push (&queue, &a));
        CATCH (queue.item_capacity != 4);
        CATCH (!queue_push (&queue, &a));
        CATCH (!queue_push (&queue, &a));
        CATCH (queue.item_capacity != 6);
        CATCH (!queue_push (&queue, &a));
        CATCH (!queue_push (&queue, &a));
        CATCH (!queue_push (&queue, &a));
        CATCH (queue.item_capacity != 9);
        queue_destroy (&queue);
        PASS ();
}

bool test_queue_grow_2 (Test *test)
{
        Queue queue;
        char a = 'a', b = 'b', c = 'c', p;

        TITLE ();
        CATCH (!queue_create (&queue, 2, sizeof (char), QUEUE_SIZE_DYNAMIC));
        CATCH (!queue_push (&queue, &a));
        CATCH (!queue_push (&queue, &b));
        CATCH (!queue_push (&queue, &c));
        CATCH (!queue_pop (&queue, &p));
        CATCH (p != 'a');
        CATCH (!queue_pop (&queue, &p));
        CATCH (p != 'b');
        CATCH (!queue_pop (&queue, &p));
        CATCH (p != 'c');
        CATCH (queue_pop (&queue, &p));
        CATCH (queue.item_capacity != 3);
        queue_destroy (&queue);
        PASS ();
}

bool test_queue_grow_3 (Test *test)
{
        Queue queue;
        char a = 'a', b = 'b', c = 'c', d = 'd', e = 'e', p;

        TITLE ();
        CATCH (!queue_create (&queue, 4, sizeof (char), QUEUE_SIZE_DYNAMIC));
        CATCH (!queue_push (&queue, &a));
        CATCH (!queue_push (&queue, &b));
        CATCH (!queue_push (&queue, &c));
        CATCH (!queue_push (&queue, &d));
        CATCH (!queue_push (&queue, &e));
        CATCH (queue.item_capacity != 6);
        CATCH (((char *)queue.items)[5] != '\0');
        CATCH (!queue_pop (&queue, &p));
        CATCH (p != 'a');
        CATCH (((char *)queue.items)[0] != '\0');
        queue_destroy (&queue);
        PASS ();
}

typedef struct _Data Data;

struct _Data {
        char x;
        char y;
        char z;
};

bool test_queue_grow_4 (Test *test)
{
        Queue queue;
        Data array[6] = {
                { .x = 'a', .y = 'b', .z = 'c' },
                { .x = 'd', .y = 'e', .z = 'f' },
                { .x = 'g', .y = 'h', .z = 'i' },
                { .x = 'j', .y = 'k', .z = 'l' },
                { .x = 'm', .y = 'n', .z = 'o' },
                { .x = 'p', .y = 'q', .z = 'r' },
        };
        Data data;

        TITLE ();
        CATCH (!queue_create (&queue, 4, sizeof (Data), QUEUE_SIZE_DYNAMIC));
        CATCH (!queue_push (&queue, &array[0]));
        CATCH (!queue_push (&queue, &array[1]));
        CATCH (!queue_push (&queue, &array[2]));
        CATCH (!queue_push (&queue, &array[3]));
        // adgj
        CATCH (!queue_pop (&queue, &data));
        // -dgj
        CATCH (!queue_push (&queue, &array[4]));
        // mdgj
        CATCH (!queue_push (&queue, &array[5]));
        // m--dgj
        // mp-dgj
        CATCH (!queue_push (&queue, &array[0]));
        // mpadgj
        //    ^
        CATCH (!queue_pop (&queue, &data));
        CATCH (data.x != 'd' || data.y != 'e' || data.z != 'f');
        // mpa-gj
        //     ^
        CATCH (!queue_pop (&queue, &data));
        CATCH (data.x != 'g' || data.y != 'h' || data.z != 'i');
        // mpa--j
        //      ^
        CATCH (!queue_pop (&queue, &data));
        CATCH (data.x != 'j' || data.y != 'k' || data.z != 'l');
        // mpa---
        // ^     
        CATCH (!queue_pop (&queue, &data));
        CATCH (data.x != 'm' || data.y != 'n' || data.z != 'o');
        // -pa---
        //  ^    
        CATCH (!queue_pop (&queue, &data));
        CATCH (data.x != 'p' || data.y != 'q' || data.z != 'r');
        // --a---
        //   ^   
        CATCH (!queue_pop (&queue, &data));
        CATCH (data.x != 'a' || data.y != 'b' || data.z != 'c');
        queue_destroy (&queue);
        PASS ();
}

bool test_queue_grow_5 (Test *test)
{
        Queue queue;
        uint64_t i, k;

        TITLE ();
        CATCH (!queue_create (&queue, 1, sizeof (uint64_t), QUEUE_SIZE_DYNAMIC));
        for (i = 1; i <= 1000000; i++) {
                CATCH (!queue_push (&queue, &i));
        }
        for (i = 1; i <= 1000000; i++) {
                CATCH (!queue_pop (&queue, &k));
                CATCH (k != i);
        }
        queue_destroy (&queue);
        PASS ();
}

bool test_queue_grow_6 (Test *test)
{
        Queue queue;
        uint64_t i, k, m, p = 1;

        TITLE ();
        CATCH (!queue_create (&queue, 1, sizeof (uint64_t), QUEUE_SIZE_DYNAMIC));
        for (i = 1; i <= 1000000;) {
                for (k = 0; k < 29; k++) {
                        CATCH (!queue_push (&queue, &i));
                        i++;
                }
                for (k = 0; k < 17; k++) {
                        CATCH (!queue_pop (&queue, &m));
                        CATCH (m != p);
                        p++;
                }
        }
        while (m < i - 1) {
                CATCH (!queue_pop (&queue, &m));
                CATCH (m != p);
                p++;
        }
        queue_destroy (&queue);
        PASS ();
}
