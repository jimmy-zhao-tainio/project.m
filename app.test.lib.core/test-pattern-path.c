#include <stdio.h>
#include <lib.core/pattern-whitespace.h>
#include <lib.core/pattern-token.h>
#include <lib.core/pattern-path.h>
#include <lib.core/string.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

#include "test-pattern-path.h"

bool test_pattern_path_buffer_size_invalid_argument_1 (Test *test)
{
        TITLE ();
        CATCH (pattern_path_buffer_size (NULL, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_pattern_path_buffer_size_invalid_argument_2 (Test *test)
{
        PatternBranch branch;

        TITLE ();
        CATCH (pattern_path_buffer_size (&branch, NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_pattern_path_buffer_size (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        size_t size;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a|b")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!pattern_path_buffer_size (branch, &size));
        CATCH (size != 123456789);
        PASS ();
}
