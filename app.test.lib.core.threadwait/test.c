#include <stdio.h>

#include <lib.test/test.h>

#include "test-threads.h"

bool (*tests[]) (Test *test) =
{
        &test_thread_signal_wait_forever,
        NULL
};

int main (int argc, char **argv)
{
        return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
