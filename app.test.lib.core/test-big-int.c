#include <lib.core/big-int.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/string.h>
#include <stdio.h>

#include "test-big-int.h"

bool test_big_int_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (BigInt) - 1);
        CATCH (big_int_create (0));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_big_int_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (BigInt) + 
                             sizeof (size_t) + 32 - 1);
        CATCH (big_int_create (0));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_big_int_create_1 (Test *test)
{
        BigInt *integer;
        char *max = "18446744073709551615";
        size_t max_length;
        size_t i;

        TITLE ();
        max_length = string_length (max);
        CATCH (!(integer = big_int_create ((uint64_t)-1)));
        CATCH (integer->digits != max_length);
        for (i = 0; i < max_length; i++) {
                CATCH (integer->memory[i] != max[i] - '0');
        }
        big_int_destroy (integer);
        PASS ();
}

bool test_big_int_create_2 (Test *test)
{
        BigInt *integer;

        TITLE ();
        CATCH (!(integer = big_int_create (0)));
        CATCH (integer->digits != 1);
        CATCH (integer->memory[0] != 0);
        big_int_destroy (integer);
        PASS ();
}

bool test_big_int_set_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (big_int_set (NULL, 123ULL));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_big_int_set (Test *test)
{
        BigInt *integer;
        char *max = "18446744073709551615";
        size_t max_length;
        size_t i;

        TITLE ();
        CATCH (!(integer = big_int_create (0)));
        CATCH (integer->digits != 1);
        CATCH (integer->memory[0] != (unsigned char)0);
        CATCH (!big_int_set (integer, (uint64_t)-1));
        max_length = string_length (max);
        CATCH (integer->digits != max_length);
        for (i = 0; i < max_length; i++) {
                CATCH (integer->memory[i] != max[i] - '0');
        }
        CATCH (!big_int_set (integer, 123));
        CATCH (integer->digits != 3);
        CATCH (integer->memory[0] != 1);
        CATCH (integer->memory[1] != 2);
        CATCH (integer->memory[2] != 3);
        big_int_destroy (integer);
        PASS ();
}

bool test_big_int_add_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (big_int_add (NULL, NULL, NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_big_int_add_invalid_argument_2 (Test *test)
{
        BigInt a;

        TITLE ();
        CATCH (big_int_add (&a, NULL, NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_big_int_add_invalid_argument_3 (Test *test)
{
        BigInt a, b;

        TITLE ();
        CATCH (big_int_add (&a, &b, NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_big_int_add_invalid_operation_1 (Test *test)
{
        BigInt a, b, to;

        TITLE ();
        a.digits = 0;
        CATCH (big_int_add (&a, &b, &to));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_big_int_add_invalid_operation_2 (Test *test)
{
        BigInt a, b, to;

        TITLE ();
        a.digits = 1;
        b.digits = 0;
        CATCH (big_int_add (&a, &b, &to));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_big_int_add_invalid_operation_3 (Test *test)
{
        BigInt a, b;

        TITLE ();
        a.digits = 1;
        b.digits = 1;
        CATCH (big_int_add (&a, &b, &a));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_big_int_add_invalid_operation_4 (Test *test)
{
        BigInt a, b;

        TITLE ();
        a.digits = 1;
        b.digits = 1;
        CATCH (big_int_add (&a, &b, &b));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 4);
        PASS ();
}

bool test_big_int_add_function_call_1 (Test *test)
{
        BigInt *a, *b, *to;
        int i;

        TITLE ();
        CATCH (!(a = big_int_create (0)));
        CATCH (!(b = big_int_create (0)));
        CATCH (!(to = big_int_create (0)));
        a->memory[0] = 5U;
        b->memory[0] = 5U;
        for (i = 1; i < 32; i++) {
                a->memory[i] = 0U;
                b->memory[i] = 0U;
        }
        a->digits = 32;
        b->digits = 32;
        memory_commit_limit (memory_commit_size ());
        CATCH (big_int_add (a, b, to));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_add_function_call_2 (Test *test)
{
        BigInt *a, *b, *to;
        int i;

        TITLE ();
        CATCH (!(a = big_int_create (0)));
        CATCH (!(b = big_int_create (0)));
        CATCH (!(to = big_int_create (0)));
        CATCH (!(a->memory = memory_grow (a->memory, 33)));
        CATCH (!(b->memory = memory_grow (b->memory, 33)));
        a->memory[0] = 1U;
        b->memory[0] = 1U;
        for (i = 1; i < 33; i++) {
                a->memory[i] = 0U;
                b->memory[i] = 0U;
        }
        a->digits = 33;
        b->digits = 33;
        memory_commit_limit (memory_commit_size ());
        CATCH (big_int_add (a, b, to));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_add_1 (Test *test)
{
        BigInt *a, *b, *to;

        TITLE ();
        CATCH (!(a = big_int_create (123)));
        CATCH (!(b = big_int_create (456)));
        CATCH (!(to = big_int_create (0)));
        CATCH (!big_int_add (a, b, to));
        CATCH (to->digits != 3);
        CATCH (to->memory[0] != 5U);
        CATCH (to->memory[1] != 7U);
        CATCH (to->memory[2] != 9U);
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_add_2 (Test *test)
{
        BigInt *a, *b, *to;
        int i;

        TITLE ();
        CATCH (!(a = big_int_create (0)));
        CATCH (!(b = big_int_create (0)));
        CATCH (!(to = big_int_create (0)));
        a->memory[0] = 5U;
        b->memory[0] = 5U;
        for (i = 1; i < 32; i++) {
                a->memory[i] = 0U;
                b->memory[i] = 0U;
        }
        a->digits = 32;
        b->digits = 32;
        CATCH (!big_int_add (a, b, to));
        CATCH (to->digits != 33);
        CATCH (to->memory[0] != 1U);
        for (i = 1; i < 33; i++) {
                CATCH (to->memory[i] != 0U);
        }
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_add_3 (Test *test)
{
        BigInt *a, *b, *to;

        TITLE ();
        CATCH (!(a = big_int_create (11)));
        CATCH (!(b = big_int_create (999)));
        CATCH (!(to = big_int_create (0)));
        CATCH (!big_int_add (a, b, to));
        CATCH (to->digits != 4);
        CATCH (to->memory[0] != 1U);
        CATCH (to->memory[1] != 0U);
        CATCH (to->memory[2] != 1U);
        CATCH (to->memory[3] != 0U);
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_add_4 (Test *test)
{
        BigInt *a, *b, *to;

        TITLE ();
        CATCH (!(a = big_int_create (999)));
        CATCH (!(b = big_int_create (11)));
        CATCH (!(to = big_int_create (0)));
        CATCH (!big_int_add (a, b, to));
        CATCH (to->digits != 4);
        CATCH (to->memory[0] != 1U);
        CATCH (to->memory[1] != 0U);
        CATCH (to->memory[2] != 1U);
        CATCH (to->memory[3] != 0U);
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_destroy_invalid_argument (Test *test)
{
        TITLE ();
        big_int_destroy (NULL);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_big_int_mul_invalid_argument_1 (Test *test)
{
        BigInt b, to;

        TITLE ();
        CATCH (big_int_mul (NULL, &b, &to));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_big_int_mul_invalid_argument_2 (Test *test)
{
        BigInt a, to;

        TITLE ();
        CATCH (big_int_mul (&a, NULL, &to));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_big_int_mul_invalid_argument_3 (Test *test)
{
        BigInt a, b;

        TITLE ();
        CATCH (big_int_mul (&a, &b, NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_big_int_mul_invalid_operation_1 (Test *test)
{
        BigInt a, b, to;

        TITLE ();
        a.digits = 0;
        b.digits = 1;
        CATCH (big_int_mul (&a, &b, &to));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_big_int_mul_invalid_operation_2 (Test *test)
{
        BigInt a, b, to;

        TITLE ();
        a.digits = 1;
        b.digits = 0;
        CATCH (big_int_mul (&a, &b, &to));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_big_int_mul_function_call_1 (Test *test)
{
        BigInt *a, *b, *to;

        TITLE ();
        CATCH (!(a = big_int_create (2)));
        CATCH (!(b = big_int_create (3)));
        CATCH (!(to = big_int_create (0)));
        memory_commit_limit (memory_commit_size ());
        CATCH (big_int_mul (a, b, to));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_mul_function_call_2 (Test *test)
{
        BigInt *a, *b, *to;

        TITLE ();
        CATCH (!(a = big_int_create (2)));
        CATCH (!(b = big_int_create (3)));
        CATCH (!(to = big_int_create (0)));
        memory_commit_limit (memory_commit_size () +
                             sizeof (size_t) + sizeof (BigInt) + sizeof (size_t) + 32 +
                             sizeof (size_t) + sizeof (BigInt) + sizeof (size_t) + 32 - 1);
        CATCH (big_int_mul (a, b, to));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_mul_function_call_3 (Test *test)
{
        BigInt *a, *b, *to;

        TITLE ();
        CATCH (!(a = big_int_create (10000000000000000000ULL)));
        CATCH (!(b = big_int_create (10000000000000000000ULL)));
        CATCH (!(to = big_int_create (0)));
        memory_commit_limit (memory_commit_size () +
                             sizeof (size_t) + sizeof (BigInt) + sizeof (size_t) + 32 +
                             sizeof (size_t) + sizeof (BigInt) + sizeof (size_t) + 32 +
                             sizeof (size_t) + sizeof (BigInt) + sizeof (size_t) + 32 - 1);
        CATCH (big_int_mul (a, b, to));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 3);
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_mul_1 (Test *test)
{
        BigInt *a, *b, *to;

        TITLE ();
        CATCH (!(a = big_int_create (12)));
        CATCH (!(b = big_int_create (34)));
        CATCH (!(to = big_int_create (0)));
        CATCH (!big_int_mul (a, b, to));
        CATCH (!big_int_equals_string (to, "408"));
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_mul_2 (Test *test)
{
        BigInt *a, *b, *to;

        TITLE ();
        CATCH (!(a = big_int_create (123451234512345)));
        CATCH (!(b = big_int_create (123451234512345)));
        CATCH (!(to = big_int_create (0)));
        CATCH (!big_int_mul (a, b, to));
        CATCH (!big_int_equals_string (to, "15240207302622001229957399025"));
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_copy_invalid_argument_1 (Test *test)
{
        BigInt to;

        TITLE ();
        CATCH (big_int_copy (NULL, &to));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_big_int_copy_invalid_argument_2 (Test *test)
{
        BigInt a;

        TITLE ();
        CATCH (big_int_copy (&a, NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_big_int_copy_invalid_operation (Test *test)
{
        BigInt *a, *to;

        TITLE ();
        CATCH (!(a = big_int_create (0)));
        CATCH (!(to = big_int_create (0)));
        a->digits = 100;
        CATCH (big_int_copy (a, to));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        big_int_destroy (a);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_copy_function_call (Test *test)
{
        BigInt *a, *b, *to;

        TITLE ();
        CATCH (!(a = big_int_create (123451234512345)));
        CATCH (!(b = big_int_create (123451234512345)));
        CATCH (!(to = big_int_create (0)));
        CATCH (!big_int_mul (a, b, to));
        CATCH (!big_int_mul (a, to, b));
        memory_commit_limit (memory_commit_size ());
        CATCH (big_int_copy (b, a));
        CATCH (error_at (0).error != ErrorFunctionCall);
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_copy (Test *test)
{
        BigInt *a, *b, *to;

        TITLE ();
        CATCH (!(a = big_int_create (123451234512345)));
        CATCH (!(b = big_int_create (123451234512345)));
        CATCH (!(to = big_int_create (0)));
        CATCH (!big_int_mul (a, b, to));
        CATCH (!big_int_copy (to, b));
        CATCH (!big_int_equals_string (b, "15240207302622001229957399025"));
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (to);
        PASS ();
}

bool test_big_int_equals_string_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (big_int_equals_string (NULL, "123"));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_big_int_equals_string_invalid_argument_2 (Test *test)
{
        BigInt a;

        TITLE ();
        CATCH (big_int_equals_string (&a, NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_big_int_equals_string (Test *test)
{
        BigInt *a;

        TITLE ();
        CATCH (!(a = big_int_create (123)));
        CATCH (big_int_equals_string (a, "1234"));
        CATCH (big_int_equals_string (a, "124"));
        CATCH (!big_int_equals_string (a, "123"));
        big_int_destroy (a);
        PASS ();
}

bool test_big_int_to_value_invalid_argument_1 (Test *test)
{
        uint64_t value;

        TITLE ();
        CATCH (big_int_to_value (NULL, &value));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_big_int_to_value_invalid_argument_2 (Test *test)
{
        BigInt a;

        TITLE ();
        CATCH (big_int_to_value (&a, NULL));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_big_int_to_value (Test *test)
{
        BigInt *a, *b, *c;
        uint64_t value;

        TITLE ();
        CATCH (!(a = big_int_create (0)));
        CATCH (!big_int_to_value (a, &value));
        CATCH (value != 0);
        big_int_destroy (a);
        CATCH (!(a = big_int_create (1)));
        CATCH (!big_int_to_value (a, &value));
        CATCH (value != 1);
        big_int_destroy (a);
        CATCH (!(a = big_int_create (10)));
        CATCH (!big_int_to_value (a, &value));
        CATCH (value != 10);
        big_int_destroy (a);
        CATCH (!(a = big_int_create (12)));
        CATCH (!big_int_to_value (a, &value));
        CATCH (value != 12);
        big_int_destroy (a);
        CATCH (!(a = big_int_create (102)));
        CATCH (!big_int_to_value (a, &value));
        CATCH (value != 102);
        big_int_destroy (a);
        CATCH (!(a = big_int_create ((uint64_t)-1)));
        CATCH (!big_int_to_value (a, &value));
        CATCH (value != (uint64_t)-1);
        big_int_destroy (a);
        CATCH (!(a = big_int_create ((uint64_t)-1)));
        CATCH (!(b = big_int_create (1)));
        CATCH (!(c = big_int_create (0)));
        CATCH (!big_int_add (a, b, c));
        CATCH (big_int_to_value (c, &value));
        big_int_destroy (a);
        big_int_destroy (b);
        big_int_destroy (c);
        PASS ();
}
