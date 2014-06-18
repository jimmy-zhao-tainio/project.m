#include <lib.core/memory.h>
#include <lib.core/string.h>
#include <lib.core/ascii.h>
#include <lib.core/error.h>
#include <lib.pattern/pattern-whitespace.h>

char *pattern_whitespace (const char *pattern)
{
	char *stripped;
	size_t length;
	size_t size;
	size_t i;

	if (!pattern) {
		error (InvalidArgument);
		return NULL;
	}
	length = string_length (pattern);
	if (!size_t_add (length, length, NULL)) {
		error (Overflow);
		return NULL;
	}
	size = 0;
	for (i = 0; i < length;) {
		if (ascii_is_white_space (pattern[i])) {
			i++;
			continue;
		}
		if (pattern[i] == '\\' && i + 1 < length && pattern[i + 1] == ' ') {
			i += 2;
			size += 2;
			continue;
		}
		i++;
		size++;
	}
	size++;
	if (!(stripped = string_create_with_size (size))) {
		error (FunctionCall);
		return NULL;
	}
	size = 0;
	for (i = 0; i < length;) {
		if (ascii_is_white_space (pattern[i])) {
			i++;
			continue;
		}
		if (pattern[i] == '\\' && i + 1 < length && pattern[i + 1] == ' ') {
			stripped[size] = pattern[i];
			stripped[size + 1] = pattern[i + 1];
			i += 2;
			size += 2;
			continue;
		}
		stripped[size] = pattern[i];
		i++;
		size++;
	}
	stripped[size] = '\0';
	return stripped;
}
