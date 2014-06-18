#include <stdio.h>
#include <lib.pattern/pattern-whitespace.h>
#include <lib.pattern/pattern-token.h>
#include <lib.pattern/pattern-path.h>
#include <lib.core/string.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

#include "test-pattern-path.h"

bool test_pattern_path_tracker_create_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (pattern_path_tracker_create (NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_pattern_path_tracker_create_function_call_1 (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () +
                             sizeof (size_t) + sizeof (PatternPathTracker) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_function_call_2 (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () +
                             sizeof (size_t) + sizeof (PatternPathTracker) +
                             sizeof (size_t) + sizeof (PatternPathValue) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () == 0);
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_branch_function_call_1 (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("(a)b")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternPathTracker) +
                             sizeof (size_t) + sizeof (PatternPathValue) +
                             sizeof (size_t) + sizeof (PatternPathValue) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () != 5);
        CATCH (error_at (2).error != ErrorFunctionCall);
        CATCH (error_at (2).code != 1);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_branch_function_call_2 (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternPathTracker) +
                             sizeof (size_t) + sizeof (PatternPathValue) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () != 4);
        CATCH (error_at (1).error != ErrorFunctionCall);
        CATCH (error_at (1).code != 2);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_branch (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        PatternPathValue *a, *b, *c, *d;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a(bc)d")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!(a = (PatternPathValue *)tracker->path));
        CATCH (a->base.type != PatternPathTypeValue);
        CATCH (a->token.value != (unsigned char)'a');
        CATCH (!(b = (PatternPathValue *)a->next));
        CATCH (b->base.type != PatternPathTypeValue);
        CATCH (b->token.value != (unsigned char)'b');
        CATCH (!(c = (PatternPathValue *)b->next));
        CATCH (c->base.type != PatternPathTypeValue);
        CATCH (c->token.value != (unsigned char)'c');
        CATCH (!(d = (PatternPathValue *)c->next));
        CATCH (d->base.type != PatternPathTypeValue);
        CATCH (d->token.value != (unsigned char)'d');
        CATCH (d->next);
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_not_function_call (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("!ab")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternPathTracker) +
                             sizeof (size_t) + sizeof (PatternPathValue) +
                             sizeof (size_t) + sizeof (PatternPathValue) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () != 5);
        CATCH (error_at (2).error != ErrorFunctionCall);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_not_range (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        PatternPathRange *range;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("![a-z]")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!(range = (PatternPathRange *)tracker->path));
        CATCH (range->base.type != PatternPathTypeRange);
        CATCH (range->token.from != (unsigned char)'a');
        CATCH (range->token.to != (unsigned char)'z');
        CATCH (range->not != true);
        CATCH (range->next);
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_not_set (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        PatternPathSet *set;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("!<abc>")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!(set = (PatternPathSet *)tracker->path));
        CATCH (set->base.type != PatternPathTypeSet);
        CATCH (set->token.values_length != 3);
        CATCH (set->token.values[0] != (unsigned char)'a');
        CATCH (set->token.values[1] != (unsigned char)'b');
        CATCH (set->token.values[2] != (unsigned char)'c');
        CATCH (set->not != true);
        CATCH (set->next);
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_not_value (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        PatternPathValue *value;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("!a")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!(value = (PatternPathValue *)tracker->path));
        CATCH (value->base.type != PatternPathTypeValue);
        CATCH (value->token.value != (unsigned char)'a');
        CATCH (value->not != true);
        CATCH (value->next);
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_or_function_call_1 (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a|b")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternPathTracker) +
                             sizeof (size_t) + sizeof (PatternPathOr) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () != 4);
        CATCH (error_at (2).error != ErrorFunctionCall);
        CATCH (error_at (2).code != 1);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_or_function_call_2 (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a|b")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternPathTracker) +
                             sizeof (size_t) + sizeof (PatternPathOr) +
                             sizeof (size_t) + sizeof (PatternPathValue) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () != 6);
        CATCH (error_at (2).error != ErrorFunctionCall);
        CATCH (error_at (2).code != 2);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_or_function_call_3 (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a|b")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternPathTracker) +
                             sizeof (size_t) + sizeof (PatternPathOr) +
                             sizeof (size_t) + sizeof (PatternPathValue) +
                             sizeof (size_t) + sizeof (PatternPathValue) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () != 6);
        CATCH (error_at (2).error != ErrorFunctionCall);
        CATCH (error_at (2).code != 3);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_or (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        PatternPathOr *or;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a|b")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!(or = (PatternPathOr *)tracker->path));
        CATCH (or->base.type != PatternPathTypeOr);
        CATCH (!or->left);
        CATCH (!or->right);
        CATCH (((PatternPathValue *)or->left)->token.value != (unsigned char)'a');
        CATCH (((PatternPathValue *)or->right)->token.value != (unsigned char)'b');
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_repeat_function_call_1 (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a{0-1}")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternPathTracker) +
                             sizeof (size_t) + sizeof (PatternPathRepeat) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () != 4);
        CATCH (error_at (2).error != ErrorFunctionCall);
        CATCH (error_at (2).code != 1);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_repeat_function_call_2 (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("(ab){0-1}c")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternPathTracker) +
                             sizeof (size_t) + sizeof (PatternPathValue) +
                             sizeof (size_t) + sizeof (PatternPathValue) +
                             sizeof (size_t) + sizeof (PatternPathValue) +
                             sizeof (size_t) + sizeof (PatternPathRepeat) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () != 6);
        CATCH (error_at (2).error != ErrorFunctionCall);
        CATCH (error_at (2).code != 2);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_repeat (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        PatternPathRepeat *repeat;
        PatternPathValue *a, *b, *c;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("(ab){0-1}c")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!(repeat = (PatternPathRepeat *)tracker->path));
        CATCH (repeat->token.from != 0);
        CATCH (repeat->token.to != 1);
        CATCH (!(a = (PatternPathValue *)repeat->repeat));
        CATCH (!(b = (PatternPathValue *)a->next));
        CATCH (!(c = (PatternPathValue *)repeat->next));
        CATCH (a->token.value != (unsigned char)'a');
        CATCH (b->token.value != (unsigned char)'b');
        CATCH (c->token.value != (unsigned char)'c');
        CATCH (b->next != (PatternPath *)repeat);
        CATCH (c->next);
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_range_function_call (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("[a-z]")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternPathTracker) +
                             sizeof (size_t) + sizeof (PatternPathRange) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () != 4);
        CATCH (error_at (2).error != ErrorFunctionCall);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_range (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        PatternPathRange *range;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("[a-z]")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!(range = (PatternPathRange *)tracker->path));
        CATCH (range->base.type != PatternPathTypeRange);
        CATCH (range->token.from != (unsigned char)'a');
        CATCH (range->token.to != (unsigned char)'z');
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_set_function_call (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("<abc>")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternPathTracker) +
                             sizeof (size_t) + sizeof (PatternPathSet) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () != 4);
        CATCH (error_at (2).error != ErrorFunctionCall);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_set (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        PatternPathSet *set;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("<abc>")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!(set = (PatternPathSet *)tracker->path));
        CATCH (set->base.type != PatternPathTypeSet);
        CATCH (set->token.values_length != 3);
        CATCH (set->token.values[0] != (unsigned char)'a');
        CATCH (set->token.values[1] != (unsigned char)'b');
        CATCH (set->token.values[2] != (unsigned char)'c');
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_value_function_call (Test *test)
{
        List *tokens;
        PatternBranch *branch;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternPathTracker) +
                             sizeof (size_t) + sizeof (PatternPathValue) - 1);
        CATCH (pattern_path_tracker_create (branch));
        CATCH (error_count () != 4);
        CATCH (error_at (2).error != ErrorFunctionCall);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_create_value (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        PatternPathValue *value;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!(value = (PatternPathValue *)tracker->path));
        CATCH (value->base.type != PatternPathTypeValue);
        CATCH (value->token.value != (unsigned char)'a');
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_tracker_destroy (Test *test)
{
        TITLE ();
        pattern_path_tracker_destroy (NULL);
        CATCH (error_count () != 0);
        PASS ();
}
