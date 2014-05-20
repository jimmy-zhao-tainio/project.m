#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <lib.core/memory.h>
#include <lib.core/error.h>

static void set_size (char *pointer, size_t size);

static unsigned long long total_create_size = 0;
static unsigned long long total_destroy_size = 0;
static unsigned long long total_create_limit = ULLONG_MAX;
static unsigned long long commit_size = 0;
static unsigned long long commit_limit = ULLONG_MAX;

void *memory_create (size_t size)
{
	char *pointer;

	if (size == 0) {
		error (InvalidArgument);
		return NULL;
	}
	if (!size_t_add (sizeof (size_t), size, &size)) {
		error (Overflow);
		return NULL;
	}
	if (!unsigned_long_long_add (size, commit_size, &commit_size)) {
		error (Overflow);
		return NULL;
	}
	if (!unsigned_long_long_add (size, total_create_size, &total_create_size)) {
		commit_size -= size;
		error (Overflow);
		return NULL;
	}
	if (commit_limit != ULLONG_MAX) {
		if (commit_size > commit_limit) {
			commit_size -= size;
			total_create_size -= size;
			error (MemoryCommitLimit);
			return NULL;
		}
	}
	if (total_create_limit != ULLONG_MAX) {
		if (total_create_size > total_create_limit) {
			commit_size -= size;
			total_create_size -= size;
			error (MemoryTotalCreateLimit);
			return NULL;
		}
	}
	if (!(pointer = calloc (size, 1))) {
		commit_size -= size;
		total_create_size -= size;
		error (OutOfMemory);
		return NULL;
	}
	set_size (pointer, size - sizeof (size_t));
	return pointer + sizeof (size_t);
}

void *memory_grow (void *memory, size_t size)
{
	char *pointer;
	char *pointer_resized;
	size_t size_current;

	if (!memory) {
		error (InvalidArgument);
		return NULL;
	}
	if (size == 0) {
		error (InvalidArgument);
		return NULL;
	}
	size_current = memory_size (memory);
	if (size_current >= size) {
		error (InvalidOperation);
		return NULL;
	}
	if (!size_t_add (sizeof (size_t), size, NULL)) {
		error (Overflow);
		return NULL;
	}
	if (!unsigned_long_long_add (size - size_current, commit_size, NULL)) {
		error (Overflow);
		return NULL;
	}
	if (!unsigned_long_long_add (size - size_current, total_create_size, NULL)) {
		error (Overflow);
		return NULL;
	}
	if (commit_limit != ULLONG_MAX) {
		if ((size - size_current) + commit_size > commit_limit) {
			error (MemoryCommitLimit);
			return NULL;
		}
	}
	if (total_create_limit != ULLONG_MAX) {
		if ((size - size_current) + total_create_size > total_create_limit) {
			error (MemoryTotalCreateLimit);
			return NULL;
		}
	}
	pointer = memory;
	pointer = pointer - sizeof (size_t);
	if (!(pointer_resized = realloc (pointer, sizeof (size_t) + size))) {
		error (OutOfMemory);
		return NULL;
	}
	total_create_size += size - size_current;
	commit_size += size - size_current;
	set_size (pointer_resized, size);
	return pointer_resized + sizeof (size_t);
}

void memory_destroy (void *memory)
{
	char *pointer;
	size_t size;
	size_t i;

	if (!memory) {
		error (InvalidArgument);
		return;
	}
	size = memory_size (memory);
	pointer = memory;
	pointer = pointer - sizeof (size_t);
	total_destroy_size += sizeof (size_t) + size;
	commit_size -= sizeof (size_t) + size;
	for (i = 0; i < sizeof (size_t) + size; i++) {
		pointer[i] = (char)rand ();
	}
	free (pointer);
}

unsigned long long memory_total_create_size (void)
{
	return total_create_size;
}

unsigned long long memory_total_destroy_size (void)
{
	return total_destroy_size;
}

unsigned long long memory_commit_size (void)
{
	return commit_size;
}

size_t memory_size (void *memory)
{
	char *pointer;
	size_t size;

	if (!memory) {
		error (InvalidArgument);
		return 0;
	}
	pointer = memory;
	pointer = pointer - sizeof (size_t);
	memcpy (&size, pointer, sizeof (size_t));
	return size;
}

static void set_size (char *pointer, size_t size)
{
	memcpy (pointer, &size, sizeof (size_t));
}

void memory_commit_limit (unsigned long long limit)
{
	commit_limit = limit;
}

void memory_total_create_limit (unsigned long long limit)
{
	total_create_limit = limit;
}

unsigned long long memory_commit_limit_get (void)
{
	return commit_limit;
}

unsigned long long memory_total_create_limit_get (void)
{
	return total_create_limit;
}

void memory_total_reset (void)
{
	total_create_size = 0;
	total_destroy_size = 0;
	commit_size = 0;
}
