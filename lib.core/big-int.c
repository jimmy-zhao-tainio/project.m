#include <lib.core/big-int.h>
#include <lib.core/ascii.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.core/string.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>

static bool digit_append_ascii (BigInt *integer, unsigned char ascii);
static bool digit_append (BigInt *integer, unsigned char value);
static void digit_reverse (BigInt *integer);
static void zero_big_int (BigInt *integer);
static bool mul_inner (BigInt *a, BigInt *b, BigInt *to, BigInt *product, BigInt *copy);
static bool pow_inner (uint64_t exponent, BigInt *bi_value, BigInt *bi_base, BigInt *bi, BigInt *product, BigInt *copy);

BigInt *big_int_create (uint64_t value)
{
        BigInt *integer;

        if (!(integer = memory_create (sizeof (BigInt)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(integer->memory = memory_create (32))) {
                memory_destroy (integer);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!big_int_set (integer, value)) {
                /* This cannot happen given the intial size of 32 digits,
                 * unless snprintf fails.
                 */
                big_int_destroy (integer);
                error_code (FunctionCall, 3);
                return NULL;
        }
        return integer;
}

bool big_int_set (BigInt *integer, uint64_t value)
{
        char buffer[32];
        size_t b;
        int written;

        if (!integer) {
                error (InvalidArgument);
                return false;
        }
        integer->digits = 0;
        written = snprintf (buffer, 32, "%llu", (unsigned long long)value);
        if (written <= 0 || written >= 32) {
                /* I don't know how to make snprintf to fail. */
                error (SystemCall);
                return false;
        }
        for (b = 0; b < 32 && buffer[b] != '\0'; b++) {
                if (!digit_append_ascii (integer, (unsigned char)buffer[b])) {
                        /* Cannot happen given the initial size. */
                        error (FunctionCall);
                        return false;
                }
        }
        return true;
}

bool big_int_add (BigInt *a, BigInt *b, BigInt *to)
{
        size_t digit_a;
        size_t digit_b;
        unsigned char value;
        unsigned char carry = 0;

        if (!a) {
                error_code (InvalidArgument, 1);
                return false;
        }
        if (!b) {
                error_code (InvalidArgument, 2);
                return false;
        }
        if (!to) {
                error_code (InvalidArgument, 3);
                return false;
        }
        if (a->digits == 0) {
                error_code (InvalidOperation, 1);
                return false;
        }
        if (b->digits == 0) {
                error_code (InvalidOperation, 2);
                return false;
        }
        if (a == to) {
                error_code (InvalidOperation, 3);
                return false;
        }
        if (b == to) {
                error_code (InvalidOperation, 4);
                return false;
        }
        to->digits = 0;
        digit_a = a->digits;
        digit_b = b->digits;
        while (true) {
                if (digit_a == 0 && digit_b == 0) {
                        if (carry == 1) {
                                if (!digit_append (to, 1)) {
                                        error_code (FunctionCall, 1);
                                        return false;
                                }
                        }
                        digit_reverse (to);
                        return true;
                }
                else if (digit_a == 0 && digit_b != 0) {
                        value = (unsigned char)(b->memory[digit_b - 1] + carry);
                        digit_b--;
                }
                else if (digit_a != 0 && digit_b == 0) {
                        value = (unsigned char)(a->memory[digit_a - 1] + carry);
                        digit_a--;
                }
                else if (digit_a != 0 && digit_b != 0) {
                        value = (unsigned char)(a->memory[digit_a - 1] + 
                                                b->memory[digit_b - 1] + carry);
                        digit_a--;
                        digit_b--;
                }
                if (value >= 10) {
                        carry = 1;
                        value = (unsigned char)(value - 10);
                }
                else {
                        carry = 0;
                }
                if (!digit_append (to, value)) {
                        error_code (FunctionCall, 2);
                        return false;
                }
        }
}

bool big_int_mul (BigInt *a, BigInt *b, BigInt *to)
{
        BigInt *product;
        BigInt *copy;
        bool success;

        if (!a) {
                error_code (InvalidArgument, 1);
                return false;
        }
        if (!b) {
                error_code (InvalidArgument, 2);
                return false;
        }
        if (!to) {
                error_code (InvalidArgument, 3);
                return false;
        }
        if (a->digits == 0) {
                error_code (InvalidOperation, 1);
                return false;
        }
        if (b->digits == 0) {
                error_code (InvalidOperation, 2);
                return false;
        }
        if (!(product = big_int_create (0))) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (!(copy = big_int_create (0))) {
                big_int_destroy (product);
                error_code (FunctionCall, 2);
                return false;
        }
        if (!(success = mul_inner (a, b, to, product, copy))) {
                error_code (FunctionCall, 3);
        }
        big_int_destroy (product);
        big_int_destroy (copy);
        return success;
}

bool big_int_pow (uint64_t base, uint64_t exponent, BigInt *to)
{
        BigInt *bi_base;
        BigInt *bi;
        BigInt *product;
        BigInt *copy;
        bool success;

        if (!to) {
                error (InvalidArgument);
                return false;
        }
        zero_big_int (to);
        to->memory[0] = 1;
        if (!(bi_base = big_int_create (base))) {
                error_code (FunctionCall, 1);
                return false;
        }
        if (!(bi = big_int_create (0))) {
                big_int_destroy (bi_base);
                error_code (FunctionCall, 2);
                return false;
        }
        if (!(product = big_int_create (0))) {
                big_int_destroy (bi_base);
                big_int_destroy (bi);
                error_code (FunctionCall, 3);
                return false;
        }
        if (!(copy = big_int_create (0))) {
                big_int_destroy (bi_base);
                big_int_destroy (bi);
                big_int_destroy (product);
                error_code (FunctionCall, 4);
                return false;
        }
        success = pow_inner (exponent, to, bi_base, bi, product, copy);
        big_int_destroy (bi_base);
        big_int_destroy (bi);
        big_int_destroy (product);
        big_int_destroy (copy);
        return success;
}

bool big_int_copy (BigInt *a, BigInt *to)
{
        void *pointer;

        if (!a) {
                error_code (InvalidArgument, 1);
                return false;
        }
        if (!to) {
                error_code (InvalidArgument, 2);
                return false;
        }
        if (memory_size (a->memory) < a->digits) {
                error (InvalidOperation);
                return false;
        }
        if (memory_size (to->memory) < memory_size (a->memory)) {
                if (!(pointer = memory_grow (to->memory, memory_size (a->memory)))) {
                        error (FunctionCall);
                        return false;
                }
                to->memory = pointer;
        }
        (void)memcpy (to->memory, a->memory, a->digits);
        to->digits = a->digits;
        return true;
}

bool big_int_equals_string (BigInt *integer, const char *string)
{
        size_t length;
        size_t i;

        if (!integer) {
                error_code (InvalidArgument, 1);
                return false;
        }
        if (!string) {
                error_code (InvalidArgument, 2);
                return false;
        }
        length = string_length (string);
        if (integer->digits != length) {
                return false;
        }
        for (i = 0; i < length; i++) {
                if (integer->memory[i] != string[i] - '0') {
                        return false;
                }
        }
        return true;
}

#define UINT64_MAX_STRING "18446744073709551615"
#define UINT64_MAX_DIGITS 20

bool big_int_to_value (BigInt *a, uint64_t *value)
{
        size_t i, t;
        uint64_t table[] = {
                10000000000000000000ULL,
                1000000000000000000ULL,
                100000000000000000ULL,
                10000000000000000ULL,
                1000000000000000ULL,
                100000000000000ULL,
                10000000000000ULL,
                1000000000000ULL,
                100000000000ULL,
                10000000000ULL,
                1000000000ULL,
                100000000ULL,
                10000000ULL,
                1000000ULL,
                100000ULL,
                10000ULL,
                1000ULL,
                100ULL,
                10ULL,
                1ULL
        };

        if (!a) {
                error_code (InvalidArgument, 1);
                return false;
        }
        if (!value) {
                error_code (InvalidArgument, 2);
                return false;
        }
        if (a->digits > UINT64_MAX_DIGITS) {
                return false;
        }
        if (a->digits == UINT64_MAX_DIGITS) {
                for (i = 0; i < UINT64_MAX_DIGITS; i++) {
                        if (a->memory[i] + '0' > UINT64_MAX_STRING[i]) {
                                return false;
                        }
                        if (a->memory[i] + '0' < UINT64_MAX_STRING[i]) {
                                break;
                        }
                }
        }
        *value = 0;
        t = UINT64_MAX_DIGITS - a->digits;
        for (i = 0; i < a->digits; i++) {
                *value += table[i + t] * a->memory[i];
        }
        return true;
}

bool big_int_print (BigInt *integer)
{
        size_t i;

        if (!integer) {
                error (InvalidArgument);
                return false;
        }
        for (i = 0; i < integer->digits; i++) {
                if (printf ("%c", integer->memory[i] + 48) <= 0) {
                        error_code (SystemCall, 1);
                        return false;
                }
        }
        return true;
}

void big_int_destroy (BigInt *integer)
{
        if (!integer) {
                error (InvalidArgument);
                return;
        }
        memory_destroy (integer->memory);
        memory_destroy (integer);
}

static bool digit_append_ascii (BigInt *integer, unsigned char ascii)
{
        if (!ascii_is_digit ((char)ascii)) {
                error (InvalidArgument);
                return false;
        }
        return digit_append (integer, (unsigned char)(ascii - '0'));
}

static bool digit_append (BigInt *integer, unsigned char value)
{
        void *pointer;
        size_t size;

        size = memory_size (integer->memory);
        if (integer->digits == size) {
                if (!size_t_add (size, size, &size)) {
                        error_code (FunctionCall, 1);
                        return false;
                }
                if (!(pointer = memory_grow (integer->memory, size))) {
                        error_code (FunctionCall, 2);
                        return false;
                }
                integer->memory = pointer;
        }
        integer->memory[integer->digits] = value;
        integer->digits++;
        return true;
}

static void digit_reverse (BigInt *integer)
{
        size_t left;
        size_t right;
        unsigned char temp;

        if (integer->digits < 2) {
                return;
        }
        left = 0;
        right = integer->digits - 1;
        while (right > left) {
                temp = integer->memory[left];
                integer->memory[left] = integer->memory[right];
                integer->memory[right] = temp;
                left++;
                right--;
        }
}

static void zero_big_int (BigInt *integer)
{
        integer->digits = 1;
        integer->memory[0] = 0;
}

static bool mul_inner (BigInt *a, BigInt *b, BigInt *to, BigInt *product, BigInt *copy)
{
        size_t digit_a;
        size_t digit_b;
        size_t zeros;
        unsigned char mul;
        unsigned char tmp;

        zero_big_int (to);
        digit_b = b->digits;
        while (digit_b --> 0) {
                digit_a = a->digits;
                while (digit_a --> 0) {
                        product->digits = 0;
                        zeros = (b->digits - digit_b - 1) + (a->digits - digit_a - 1);
                        while (zeros --> 0) {
                                if (!digit_append (product, 0)) {
                                        error_code (FunctionCall, 1);
                                        return false;
                                }
                        }
                        mul = (unsigned char)(b->memory[digit_b] * a->memory[digit_a]);
                        if (mul < 10) {
                                if (!digit_append (product, mul)) {
                                        error_code (FunctionCall, 2);
                                        return false;
                                }
                        }
                        else {
                                tmp = mul / 10;
                                if (!digit_append (product, (unsigned char)(mul - (tmp * 10)))) {
                                        error_code (FunctionCall, 3);
                                        return false;
                                }
                                if (!digit_append (product, tmp)) {
                                        error_code (FunctionCall, 4);
                                        return false;
                                }
                        }
                        digit_reverse (product);
                        if (!big_int_copy (to, copy)) {
                                error_code (FunctionCall, 5);
                                return false;
                        }
                        if (!big_int_add (product, copy, to)) {
                                error_code (FunctionCall, 6);
                                return false;
                        }
                }
        }
        return true;
}

static bool pow_inner (uint64_t exponent, BigInt *bi_value, BigInt *bi_base, BigInt *bi, BigInt *product, BigInt *copy)
{
        while (exponent != 0) {
                if (exponent % 2 == 1) {
                        if (!mul_inner (bi_value, bi_base, bi, product, copy)) {
                                return false;
                        }
                        if (!big_int_copy (bi, bi_value)) {
                                return false;
                        }
                }
                exponent /= 2;
                if (!mul_inner (bi_base, bi_base, bi, product, copy)) {
                        return false;
                }
                if (!big_int_copy (bi, bi_base)) {
                        return false;
                }     
        }
        return true;
}
