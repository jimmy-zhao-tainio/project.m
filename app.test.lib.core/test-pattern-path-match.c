#include <lib.core/pattern-path.h>
#include <lib.core/pattern-path-match.h>
#include <lib.core/string.h>
#include <lib.core/error.h>

#include "test-pattern-path-match.h"

bool test_pattern_path_match_1 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (pattern_path_match (tracker, "", string_length (""), &match_to));
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_2 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!pattern_path_match (tracker, "a", string_length ("a"), &match_to));
        CATCH (match_to != 1);
        CATCH (!pattern_path_match (tracker, "a", string_length ("a"), &match_to));
        CATCH (match_to != 1);
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_not_1 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("!ab!cd")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (pattern_path_match (tracker, "abxd", string_length ("abxd"), &match_to));
        CATCH (pattern_path_match (tracker, "xbcd", string_length ("xbcd"), &match_to));
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_not_2 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("!ab!cd")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!pattern_path_match (tracker, "xbxd", string_length ("xbxd"), &match_to));
        CATCH (match_to != 4);
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_or (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a|b|c|d")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!pattern_path_match (tracker, "a", string_length ("a"), &match_to));
        CATCH (match_to != 1);
        CATCH (!pattern_path_match (tracker, "b", string_length ("b"), &match_to));
        CATCH (match_to != 1);
        CATCH (!pattern_path_match (tracker, "c", string_length ("c"), &match_to));
        CATCH (match_to != 1);
        CATCH (!pattern_path_match (tracker, "d", string_length ("d"), &match_to));
        CATCH (match_to != 1);
        CATCH (pattern_path_match (tracker, "e", string_length ("e"), &match_to));
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_repeat_1 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a{4}b")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (pattern_path_match (tracker, "aaab", string_length ("aaab"), &match_to));
        CATCH (pattern_path_match (tracker, "aaaa", string_length ("aaaa"), &match_to));
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_repeat_2 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a{2-4}b")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (pattern_path_match (tracker, "abbb", string_length ("abbb"), &match_to));
        CATCH (pattern_path_match (tracker, "aaaaabbb", string_length ("aaaaabbb"), &match_to));
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_repeat_3 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("(a|a){-}(a|a){-}b")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (pattern_path_match (tracker, "aaaaaaaac", string_length ("aaaaaaaac"), &match_to));
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_repeat_4 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a{4}b")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!pattern_path_match (tracker, "aaaab", string_length ("aaaab"), &match_to));
        CATCH (match_to != 5);
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_repeat_5 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("a{2-4}b")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!pattern_path_match (tracker, "aabb", string_length ("aabb"), &match_to));
        CATCH (match_to != 3);
        CATCH (!pattern_path_match (tracker, "aaabb", string_length ("aaabb"), &match_to));
        CATCH (match_to != 4);
        CATCH (!pattern_path_match (tracker, "aaaabb", string_length ("aaaabb"), &match_to));
        CATCH (match_to != 5);
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_repeat_6 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("(a|a){-}(a|a){-}b")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!pattern_path_match (tracker, "aaaab", string_length ("aaaab"), &match_to));
        CATCH (match_to != 5);
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_range_1 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("[a-z]")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (pattern_path_match (tracker, "`", string_length ("`"), &match_to));
        CATCH (pattern_path_match (tracker, "{", string_length ("{"), &match_to));
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_range_2 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("[a-z]")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!pattern_path_match (tracker, "a", string_length ("a"), &match_to));
        CATCH (match_to != 1);
        CATCH (!pattern_path_match (tracker, "z", string_length ("z"), &match_to));
        CATCH (match_to != 1);
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_set_1 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("<ac>")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (pattern_path_match (tracker, "b", string_length ("b"), &match_to));
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}

bool test_pattern_path_match_set_2 (Test *test)
{
        List *tokens;
        PatternBranch *branch;
        PatternPathTracker *tracker;
        size_t match_to;

        TITLE ();
        CATCH (!(tokens = pattern_tokens_create ("<ac>")));
        CATCH (!(branch = pattern_branch_create (tokens)));
        CATCH (!(tracker = pattern_path_tracker_create (branch)));
        CATCH (!pattern_path_match (tracker, "a", string_length ("a"), &match_to));
        CATCH (match_to != 1);
        CATCH (!pattern_path_match (tracker, "c", string_length ("c"), &match_to));
        CATCH (match_to != 1);
        pattern_path_tracker_destroy (tracker);
        pattern_branch_destroy (branch);
        pattern_tokens_destroy (tokens);
        PASS ();
}
