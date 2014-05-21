#include <lib.core/pattern-path.h>
#include <lib.core/error.h>

PatternPath *pattern_path_create (PatternBranch *branch)
{
        PatternPath *first = NULL;
        size_t i;

        if (!branch) {
                error (InvalidArgument);
                return NULL;
        }
        for (i = 0; i < list_count (branch->parts); i++) {
                
        }
        return first;
}
