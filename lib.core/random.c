#include <lib.core/random.h>
#include <lib.core/error.h>
#include <stdio.h>
#include <strings.h>

static FILE *file = NULL;

static bool read_bits_to_integer (unsigned int bits, unsigned long long *value);
static void bits_integer_copy (unsigned long long *source, unsigned int source_index,
			       unsigned long long *destination, unsigned int destination_index,
			       unsigned int bits);

bool random_open (void)
{
	if (file) {
		error (InvalidOperation);
		return false;
	}
	if (!(file = fopen ("/dev/urandom", "rb"))) {
		error (SystemCall);
		return false;
	}
	return true;
}

bool random_bytes (unsigned char *bytes, size_t bytes_length)
{
	size_t read_length = 0;

	if (!file) {
		error (InvalidOperation);
		return false;
	}
	if (!bytes) {
		error_code (InvalidArgument, 1);
		return false;
	}
	if (bytes_length == 0) {
		error_code (InvalidArgument, 2);
		return false;
	}
	while (true) {
		read_length += fread (bytes + read_length, 1, bytes_length - read_length, file);
		if (read_length >= bytes_length) {
			return true;
		}
		if (feof (file) != 0 || ferror (file) != 0) {
			error (SystemCall);
			return false;
		}
	}
}

bool random_value (unsigned long long from, unsigned long long to, unsigned long long *value)
{
	unsigned long long range = to - from;
	unsigned int most_significant_bit;

	if (to < from) {
		error (InvalidOperation);
		return false;
	}
	if (range == 0) {
		*value = from;
		return true;
	}
	most_significant_bit = unsigned_long_long_bit_most_significant (range);
	do {
		if (!read_bits_to_integer (most_significant_bit + 1, value)) {
			return false;
		}
	} while (*value > range);
	*value += from;
	return true;
}

static unsigned long long source = 0;
static unsigned int source_index = 64;

static bool read_bits_to_integer (unsigned int bits, unsigned long long *value)
{
	unsigned int read = 0;
	unsigned int copy;
	
	*value = 0;
	while (read != bits) {
		if (source_index == 64) {
			if (!random_bytes ((unsigned char *)&source, 8)) {
				return false;
			}
			source_index = 0;
		}
		copy = 64 - source_index < bits - read ? 64 - source_index : bits - read;
		bits_integer_copy (&source, source_index, value, read, copy);
		read += copy;
		source_index += copy;
	}
	return true;
}

static void bits_integer_copy (unsigned long long *source, unsigned int source_index,
			       unsigned long long *destination, unsigned int destination_index,
			       unsigned int bits)
{
	unsigned int i;

	for (i = 0; i < bits; i++) {
		if (unsigned_long_long_bit_get (*source, source_index + i)) {
			*destination = unsigned_long_long_bit_set (*destination, destination_index + i, 1);
		}
		else {
			*destination = unsigned_long_long_bit_set (*destination, destination_index + i, 0);
		}
	}
}

void random_close (void)
{
	if (!file) {
		error (InvalidOperation);
		return;
	}
	if (fclose (file) != 0) {
		file = NULL;
		error (SystemCall);
		return;
	}
	file = NULL;
	source = 0;
	source_index = 64;
	return;
}

bool random_is_open (void)
{
        if (!file) {
                return false;
        }
        return true;
}
