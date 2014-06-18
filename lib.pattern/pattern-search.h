#ifndef pattern_search_h
#define pattern_search_h

#include <lib.core/defines.h>
#include <lib.pattern/pattern-token.h>
#include <lib.pattern/pattern-branch.h>
#include <lib.pattern/pattern-path.h>

typedef struct s_PatternSearch PatternSearch;

struct s_PatternSearch
{
        List *tokens;
        PatternBranch *branch;
        PatternPath *path;
        PatternPathTracker *tracker;
        const unsigned char *buffer;
        size_t buffer_length;
        size_t from;
};

PatternSearch *pattern_search_create (const unsigned char *buffer, size_t buffer_length, const char *pattern, bool strip_whitespace);
void pattern_search_destroy (PatternSearch *search);
bool pattern_search_next (PatternSearch *search, size_t *match_from, size_t *match_to);

#endif
