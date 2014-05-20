#include <stdio.h>
#include <lib.core/memory.h>
#include <lib.core/permute.h>
#include <lib.core/permute-private.h>
#include <lib.core/error.h>

Permute *permute_create (size_t length)
{
	Permute *permute;
	size_t i;

	if (length == 0) {
		error (InvalidArgument);
		return NULL;
	}
	if (!size_t_add (length, 1, NULL)) {
		error (Overflow);
		return NULL;
	}
	if (!size_t_mul (length + 1, sizeof (size_t), NULL)) {
		error (Overflow);
		return NULL;
	}
	if (!(permute = memory_create (sizeof (Permute)))) {
		error (FunctionCall);
		return NULL;
	}
	if (!(permute->array = memory_create (sizeof (size_t) * (length + 1)))) {
		memory_destroy (permute);
		error (FunctionCall);
		return NULL;
	}
	for (i = 0; i <= length; i++) {
		permute->array[i] = i == 0 ? 1 : 0;
	}
	permute->length = length;
	return permute;
}

void permute_destroy (Permute *permute)
{
	if (!permute) {
		error (InvalidArgument);
		return;
	}
	if (permute->array) {
		memory_destroy (permute->array);
	}
	memory_destroy (permute);
}

const size_t *permute_next (Permute *permute)
{
	size_t i, j, k;

	if (!permute) {
		error (InvalidArgument);
		return NULL;
	}
	if (*(permute->array)) { 
		for (i = 0; i <= permute->length; i++) {
			permute->array[i] = i; 
		}
		return permute->array + 1; 
	}
	for (i = permute->length - 1; i > 0 && permute->array[i] > permute->array[i + 1]; i--) { }
	if (i == 0) {
		return NULL;
	}
	j = i;
	for (i = permute->length; permute->array[i] < permute->array[j]; i--) { }
	k = permute->array[j];
	permute->array[j] = permute->array[i];
	permute->array[i] = k;
	for (i = permute->length, j++; j < i; j++, i--) { 
		k = permute->array[j]; 
		permute->array[j] = permute->array[i]; 
		permute->array[i] = k; 
	}
	return permute->array + 1;
}
