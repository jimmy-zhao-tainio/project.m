#include <lib.core/memory.h>
#include <lib.core/string.h>
#include <lib.core/list.h>
#include <lib.core/error.h>
#include <lib.core/pattern-whitespace.h>
#include <lib.core/pattern-token.h>
#include <lib.core/pattern-branch.h>
#include <lib.core/pattern-path.h>
#include <lib.core/pattern-path-match.h>
#include <lib.core/pattern-search.h>

PatternSearch *pattern_search_create (const unsigned char *buffer, size_t buffer_length, const char *pattern, bool strip_whitespace)
{
        PatternSearch *search;
        char *stripped;

        if (!(search = memory_create (sizeof (PatternSearch)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        search->buffer = buffer;
        search->buffer_length = buffer_length;
        search->from = 0;
        if (strip_whitespace) {
                if (!(stripped = pattern_whitespace (pattern))) {
                        pattern_search_destroy (search);
                        error_code (FunctionCall, 2);
                        return NULL;
                }
                if (!(search->tokens = pattern_tokens_create (stripped))) {
			string_destroy (stripped);
			pattern_search_destroy (search);
			error_code (FunctionCall, 3);
			return NULL;
		}
		string_destroy (stripped);
	}
	else {
		if (!(search->tokens = pattern_tokens_create (pattern))) {
			pattern_search_destroy (search);
			error_code (FunctionCall, 4);
			return NULL;
		}
	}
	if (!(search->branch = pattern_branch_create (search->tokens))) {
		pattern_search_destroy (search);
		error_code (FunctionCall, 5);
		return NULL;
	}
        if (!(search->tracker = pattern_path_tracker_create (search->branch))) {
                pattern_search_destroy (search);
                error_code (FunctionCall, 6);
                return NULL;
        }
	return search;
}

void pattern_search_destroy (PatternSearch *search)
{
	if (!search) {
		return;
	}
	if (search->tokens) {
		pattern_tokens_destroy (search->tokens);
	}
	if (search->branch) {
		pattern_branch_destroy (search->branch);
	}
        if (search->tracker) {
                pattern_path_tracker_destroy (search->tracker);
        }
	memory_destroy (search);
}

bool pattern_search_next (PatternSearch *search, size_t *match_from, size_t *match_to)
{
	size_t to;

	while (search->from < search->buffer_length - 1) {
		if (pattern_path_match (search->tracker, (const char *)search->buffer, search->buffer_length, search->from, &to)) {
			*match_from = search->from;
			*match_to = to;
			if (search->from == to) {
				search->from++;
			}
			else {
				search->from = to;
			}
			return true;
		}
		search->from++;
	}
	return false;
}
