#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.core/threads-lock.h>

static void set_size (char *pointer, size_t size);

static unsigned long long total_create_size = 0;
static unsigned long long total_destroy_size = 0;
static unsigned long long total_create_limit = ULLONG_MAX;
static unsigned long long commit_size = 0;
static unsigned long long commit_limit = ULLONG_MAX;

static ThreadLock lock = THREAD_LOCK_INITIALIZER;

#define LOCK() \
        if (!thread_lock (&lock)) { \
                error (FunctionCall); \
        }
#define UNLOCK() \
        if (!thread_unlock (&lock)) { \
                error (FunctionCall); \
        }

void *memory_create (size_t size)
{
	char *pointer;

	if (size == 0) {
		error (InvalidArgument);
		return NULL;
	}
	if (!size_t_add (sizeof (size_t), size, &size)) {
		error_code (Overflow, 1);
		return NULL;
	}
        LOCK ();
	if (!unsigned_long_long_add (size, commit_size, &commit_size)) {
                UNLOCK ();
		error_code (Overflow, 2);
		return NULL;
	}
	if (!unsigned_long_long_add (size, total_create_size, &total_create_size)) {
		commit_size -= size;
                UNLOCK ();
		error_code (Overflow, 3);
		return NULL;
	}
	if (commit_limit != ULLONG_MAX) {
		if (commit_size > commit_limit) {
			commit_size -= size;
			total_create_size -= size;
                        UNLOCK ();
			error (MemoryCommitLimit);
			return NULL;
		}
	}
	if (total_create_limit != ULLONG_MAX) {
		if (total_create_size > total_create_limit) {
			commit_size -= size;
			total_create_size -= size;
			error (MemoryTotalCreateLimit);
                        UNLOCK ();
			return NULL;
		}
	}
        UNLOCK ();
	if (!(pointer = calloc (size, 1))) {
                LOCK ();
		commit_size -= size;
		total_create_size -= size;
                UNLOCK ();
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
		error_code (Overflow, 1);
		return NULL;
	}
        LOCK ();
	if (!unsigned_long_long_add (size - size_current, commit_size, NULL)) {
                UNLOCK ();
		error_code (Overflow, 2);
		return NULL;
	}
	if (!unsigned_long_long_add (size - size_current, total_create_size, NULL)) {
                UNLOCK ();
		error_code (Overflow, 3);
		return NULL;
	}
	if (commit_limit != ULLONG_MAX) {
		if ((size - size_current) + commit_size > commit_limit) {
                        UNLOCK ();
			error (MemoryCommitLimit);
			return NULL;
		}
	}
	if (total_create_limit != ULLONG_MAX) {
		if ((size - size_current) + total_create_size > total_create_limit) {
                        UNLOCK ();
			error (MemoryTotalCreateLimit);
			return NULL;
		}
	}
        total_create_size += size - size_current;
	commit_size += size - size_current;
        UNLOCK ();

	pointer = memory;
	pointer = pointer - sizeof (size_t);
	if (!(pointer_resized = realloc (pointer, sizeof (size_t) + size))) {
                LOCK ();
                total_create_size -= size - size_current;
                commit_size -= size - size_current;
                UNLOCK ();
		error (OutOfMemory);
		return NULL;
	}
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
        LOCK ();
	total_destroy_size += sizeof (size_t) + size;
	commit_size -= sizeof (size_t) + size;
        UNLOCK ();
	for (i = 0; i < sizeof (size_t) + size; i++) {
		pointer[i] = (char)rand ();
	}
	free (pointer);
}

unsigned long long memory_total_create_size (void)
{
        unsigned long long tmp;

        LOCK ();
        tmp = total_create_size;
        UNLOCK ();
	return tmp;
}

unsigned long long memory_total_destroy_size (void)
{
        unsigned long long tmp;

        LOCK ();
        tmp = total_destroy_size;
        UNLOCK ();
	return tmp;
}

unsigned long long memory_commit_size (void)
{
        unsigned long long tmp;

        LOCK ();
        tmp = commit_size;
        UNLOCK ();
	return tmp;
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
        LOCK ();
	commit_limit = limit;
        UNLOCK ();
}

void memory_total_create_limit (unsigned long long limit)
{
        LOCK ();
	total_create_limit = limit;
        UNLOCK ();
}

unsigned long long memory_commit_limit_get (void)
{
        unsigned long long tmp;

        LOCK ();
        tmp = commit_limit;
        UNLOCK ();
	return tmp;
}

unsigned long long memory_total_create_limit_get (void)
{
        unsigned long long tmp;

        LOCK ();
        tmp = total_create_limit;
        UNLOCK ();
	return tmp;
}

void memory_total_reset (void)
{
        LOCK ();
	total_create_size = 0;
	total_destroy_size = 0;
	commit_size = 0;
        UNLOCK ();
}
