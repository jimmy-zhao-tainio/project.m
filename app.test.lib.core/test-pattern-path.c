#include <stdio.h>
#include <lib.core/pattern-whitespace.h>
#include <lib.core/pattern-token.h>
#include <lib.core/pattern-path.h>
#include <lib.core/string.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

#include "test-pattern-path.h"

bool test_pattern_path_create_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (pattern_path_create (NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_pattern_path_create_function_call (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size ());
        CATCH (pattern_path_create (branch));
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_create (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPath *path;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(path = pattern_path_create (branch)));
        pattern_path_destroy (path);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}
