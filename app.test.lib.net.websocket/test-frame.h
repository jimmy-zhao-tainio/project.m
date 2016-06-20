#ifndef test_frame_h
#define test_frame_h

#include <lib.test/test.h>

bool test_frame_create_function_call (Test *test);
bool test_frame_create (Test *test);
bool test_frame_append_invalid_operation (Test *test);
bool test_frame_append_function_call (Test *test);
bool test_frame_append (Test *test);

#endif
