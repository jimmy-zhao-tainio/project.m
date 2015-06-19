#include <stdio.h>
#include <lib.pattern/pattern-whitespace.h>
#include <lib.pattern/pattern-token.h>
#include <lib.pattern/pattern-branch.h>
#include <lib.core/string.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

#include "test-pattern-branch.h"

bool test_pattern_branch_create_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (pattern_branch_create (NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_pattern_branch_create_function_call_1 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
	memory_commit_limit (memory_commit_size ());
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 3);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 1);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_function_call_2 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
	memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 3);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 2);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_invalid_operation (Test *test)
{
	List *tokens;
	PatternBranch *branch;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
	CATCH (!(branch = pattern_branch_create (tokens)));
	pattern_branch_destroy (branch);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create (Test *test)
{
	List *tokens;
	PatternBranch *branch;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
	CATCH (!(branch = pattern_branch_create (tokens)));
	pattern_branch_destroy (branch);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_destroy_invalid_argument (Test *test)
{
	TITLE ();
	pattern_branch_destroy (NULL);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_pattern_branch_create_parenthesize_main_branch_function_call_1 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 3);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (1).code != 1);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_parenthesize_main_branch_function_call_2 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (1).code != 2);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_parenthesize_main_branch_function_call_3 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 3);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (1).code != 3);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_parenthesize_main_branch_function_call_4 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (1).code != 4);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_parenthesize_main_branch (Test *test)
{
	List *tokens;
	PatternBranch *branch;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
	CATCH (!(branch = pattern_branch_create (tokens)));
	CATCH (list_count (branch->parts) != 1);
	pattern_branch_destroy (branch);
	pattern_tokens_destroy (tokens);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_pattern_branch_create_branch_create_function_call_1 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 3);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (1).code != 1);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_branch_create_function_call_2 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (1).code != 2);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_branch_create_function_call_3 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("(a)")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartValue) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (1).code != 3);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_branch_create_function_call_4 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a|b")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartValue) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranchPartOr) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (ListNode) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (1).code != 4);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_branch_create_function_call_5 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartValue) +
                             sizeof (size_t) + sizeof (ListNode) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (1).error != ErrorFunctionCall);
	CATCH (error_at (1).code != 5);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_branch_recursion_depth (Test *test)
{
	char *input;
	size_t i;
	List *tokens;

	TITLE ();
	CATCH (!(input = string_create_with_size (PATTERN_BRANCH_RECURSION_DEPTH + 1 +
                                                  1 +
						  PATTERN_BRANCH_RECURSION_DEPTH + 1 + 
						  1)));
	for (i = 0; i < PATTERN_BRANCH_RECURSION_DEPTH + 1; i++) {
		input[i] = '(';
	}
        input[i] = 'a';
        i++;
	for (; i < (2 * (PATTERN_BRANCH_RECURSION_DEPTH + 1)) + 1; i++) {
		input[i] = ')';
	}
	input[(2 * (PATTERN_BRANCH_RECURSION_DEPTH + 1)) + 1] = '\0';
	CATCH (!(tokens = pattern_tokens_create (input)));
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorPatternBranchRecursionDepth);
	CATCH (error_at (0).code != 2);
	pattern_tokens_destroy (tokens);
	string_destroy (input);
	PASS ();
}

bool test_pattern_branch_create_part_branch_function_call_1 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("(a)")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartBranch) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (2).error != ErrorFunctionCall);
	CATCH (error_at (2).code != 1);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_branch_function_call_2 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("(a)")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartBranch) +
                             sizeof (size_t) + sizeof (PatternBranch) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 5);
	CATCH (error_at (2).error != ErrorFunctionCall);
	CATCH (error_at (2).code != 2);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_not_function_call_1 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("!a")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartNot) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (2).error != ErrorFunctionCall);
	CATCH (error_at (2).code != 1);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_not_function_call_2 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("!a")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartNot));
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 5);
	CATCH (error_at (2).error != ErrorFunctionCall);
	CATCH (error_at (2).code != 2);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_or_function_call_1 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a|b")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartValue) +
                             sizeof (size_t) + sizeof (ListNode) + 
                             sizeof (size_t) + sizeof (PatternBranchPartOr) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (2).error != ErrorFunctionCall);
	CATCH (error_at (2).code != 1);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_or_function_call_2 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a|b")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartValue) +
                             sizeof (size_t) + sizeof (ListNode) + 
                             sizeof (size_t) + sizeof (PatternBranchPartOr) +
                             sizeof (size_t) + sizeof (PatternBranch) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (2).error != ErrorFunctionCall);
	CATCH (error_at (2).code != 2);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_or_function_call_3 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a|b")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartValue) +
                             sizeof (size_t) + sizeof (ListNode) + 
                             sizeof (size_t) + sizeof (PatternBranchPartOr) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 5);
	CATCH (error_at (2).error != ErrorFunctionCall);
	CATCH (error_at (2).code != 3);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_or_function_call_4 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a|b")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartValue) +
                             sizeof (size_t) + sizeof (ListNode) + 
                             sizeof (size_t) + sizeof (PatternBranchPartOr) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranch) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (2).error != ErrorFunctionCall);
	CATCH (error_at (2).code != 4);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_or_function_call_5 (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a|b")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartValue) +
                             sizeof (size_t) + sizeof (ListNode) + 
                             sizeof (size_t) + sizeof (PatternBranchPartOr) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 5);
	CATCH (error_at (2).error != ErrorFunctionCall);
	CATCH (error_at (2).code != 5);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_repeat_function_call (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a{0-1}")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartValue) +
                             sizeof (size_t) + sizeof (ListNode) + 
                             sizeof (size_t) + sizeof (PatternBranchPartRepeat) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (2).error != ErrorFunctionCall);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_range_function_call (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("[a-b]")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartRange) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (2).error != ErrorFunctionCall);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_set_function_call (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("<a>")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartSet) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (2).error != ErrorFunctionCall);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_part_value_function_call (Test *test)
{
	List *tokens;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a")));
        memory_commit_limit (memory_commit_size () + 
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternToken) +
                             sizeof (size_t) + sizeof (ListNode) +
                             sizeof (size_t) + sizeof (PatternBranch) +
                             sizeof (size_t) + sizeof (List) +
                             sizeof (size_t) + sizeof (PatternBranchPartValue) - 1);
	CATCH (pattern_branch_create (tokens));
	CATCH (error_count () != 4);
	CATCH (error_at (2).error != ErrorFunctionCall);
	pattern_tokens_destroy (tokens);
	PASS ();
}

bool test_pattern_branch_create_1 (Test *test)
{
	List *tokens;
	PatternBranch *branch;
	PatternBranchPartBranch *part_branch;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("(a)")));
	CATCH (!(branch = pattern_branch_create (tokens)));
	CATCH (!branch->parts);
	CATCH (list_count (branch->parts) != 1);
	CATCH (!(part_branch = (PatternBranchPartBranch *)list_first (branch->parts)->data));
	CATCH (!part_branch->branch);
	CATCH (!part_branch->branch->parts);
	CATCH (list_count (part_branch->branch->parts) != 1);
	pattern_branch_destroy (branch);
	pattern_tokens_destroy (tokens);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_pattern_branch_create_2 (Test *test)
{
	List *tokens;
	PatternBranch *branch;
	PatternBranchPartValue *part_a;
	PatternBranchPartBranch *part_branch;
	PatternBranchPartValue *part_branch_part_b;
	PatternBranchPartValue *part_c;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a(b)c")));
	CATCH (!(branch = pattern_branch_create (tokens)));
	CATCH (!branch->parts);
	CATCH (list_count (branch->parts) != 3);
	CATCH (!(part_a = (PatternBranchPartValue *)list_first (branch->parts)->data));
	CATCH (part_a->token->value != 'a');
	CATCH (!(part_branch = (PatternBranchPartBranch *)list_next (list_first (branch->parts))->data));
	CATCH (!part_branch->branch);
	CATCH (!part_branch->branch->parts);
	CATCH (list_count (part_branch->branch->parts) != 1);
	CATCH (!(part_branch_part_b = (PatternBranchPartValue *)list_first (part_branch->branch->parts)->data));
	CATCH (part_branch_part_b->token->value != 'b');
	CATCH (!(part_c = (PatternBranchPartValue *)list_next (list_next (list_first (branch->parts)))->data));
	CATCH (part_c->token->value != 'c');
	pattern_branch_destroy (branch);
	pattern_tokens_destroy (tokens);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_pattern_branch_create_3 (Test *test)
{
	List *tokens;
	PatternBranch *branch;
	PatternBranchPartNot *part_not;
	PatternBranchPartValue *part_a;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("!a")));
	CATCH (!(branch = pattern_branch_create (tokens)));
	CATCH (!branch->parts);
	CATCH (list_count (branch->parts) != 1);
	CATCH (!(part_not = (PatternBranchPartNot *)list_first (branch->parts)->data));
	CATCH (!(part_a = (PatternBranchPartValue *)part_not->part));
	CATCH (part_a->token->value != 'a');
	pattern_branch_destroy (branch);
	pattern_tokens_destroy (tokens);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_pattern_branch_create_4 (Test *test)
{
	List *tokens;
	PatternBranch *branch;
	PatternBranchPartNot *part_not;
	PatternBranchPartValue *part_a;
	PatternBranchPartValue *part_b;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("!ab")));
	CATCH (!(branch = pattern_branch_create (tokens)));
	CATCH (!branch->parts);
	CATCH (list_count (branch->parts) != 2);
	CATCH (!(part_not = (PatternBranchPartNot *)list_first (branch->parts)->data));
	CATCH (!(part_a = (PatternBranchPartValue *)part_not->part));
	CATCH (part_a->token->value != 'a');
	CATCH (!(part_b = (PatternBranchPartValue *)list_next (list_first (branch->parts))->data));
	CATCH (part_b->token->value != 'b');
	pattern_branch_destroy (branch);
	pattern_tokens_destroy (tokens);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_pattern_branch_create_5 (Test *test)
{
	List *tokens;
	PatternBranch *branch;
	PatternBranchPartOr *part_or;
	PatternBranchPartValue *part_a;
	PatternBranchPartValue *part_b;
	PatternBranchPartValue *part_c;
	PatternBranchPartValue *part_d;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("ab|cd")));
	CATCH (!(branch = pattern_branch_create (tokens)));
	CATCH (!branch->parts);
	CATCH (list_count (branch->parts) != 1);
	CATCH (!(part_or = (PatternBranchPartOr *)list_first (branch->parts)->data));
	CATCH (!(part_a = (PatternBranchPartValue *)list_first (part_or->left->parts)->data));
	CATCH (!(part_b = (PatternBranchPartValue *)list_last (part_or->left->parts)->data));
	CATCH (part_a->token->value != 'a');
	CATCH (part_b->token->value != 'b');
	CATCH (!(part_c = (PatternBranchPartValue *)list_first (part_or->right->parts)->data));
	CATCH (!(part_d = (PatternBranchPartValue *)list_last (part_or->right->parts)->data));
	CATCH (part_c->token->value != 'c');
	CATCH (part_d->token->value != 'd');
	pattern_branch_destroy (branch);
	pattern_tokens_destroy (tokens);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_pattern_branch_create_6 (Test *test)
{
	List *tokens;
	PatternBranch *branch;
	PatternBranchPartOr *part_or_1;
	PatternBranchPartOr *part_or_2;
	PatternBranchPartValue *part_a;
	PatternBranchPartValue *part_b;
	PatternBranchPartValue *part_c;
	PatternBranchPartValue *part_d;
	PatternBranchPartValue *part_e;
	PatternBranchPartValue *part_f;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("ab|cd|ef")));
	CATCH (!(branch = pattern_branch_create (tokens)));
	CATCH (!branch->parts);
	CATCH (list_count (branch->parts) != 1);
	CATCH (!(part_or_1 = (PatternBranchPartOr *)list_first (branch->parts)->data));
	CATCH (list_count (part_or_1->left->parts) != 1);
	CATCH (!(part_or_2 = (PatternBranchPartOr *)list_first (part_or_1->left->parts)->data));
	CATCH (!(part_a = (PatternBranchPartValue *)list_first (part_or_2->left->parts)->data));
	CATCH (!(part_b = (PatternBranchPartValue *)list_last (part_or_2->left->parts)->data));
	CATCH (part_a->token->value != 'a');
	CATCH (part_b->token->value != 'b');
	CATCH (!(part_c = (PatternBranchPartValue *)list_first (part_or_2->right->parts)->data));
	CATCH (!(part_d = (PatternBranchPartValue *)list_last (part_or_2->right->parts)->data));
	CATCH (part_c->token->value != 'c');
	CATCH (part_d->token->value != 'd');
	CATCH (!(part_e = (PatternBranchPartValue *)list_first (part_or_1->right->parts)->data));
	CATCH (!(part_f = (PatternBranchPartValue *)list_last (part_or_1->right->parts)->data));
	CATCH (part_e->token->value != 'e');
	CATCH (part_f->token->value != 'f');
	pattern_branch_destroy (branch);
	pattern_tokens_destroy (tokens);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_pattern_branch_create_7 (Test *test)
{
	List *tokens;
	PatternBranch *branch;
	PatternBranchPartRepeat *part_repeat;
	PatternBranchPartValue *part_a;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("a{0-2}")));
	CATCH (!(branch = pattern_branch_create (tokens)));
	CATCH (!branch->parts);
	CATCH (list_count (branch->parts) != 1);
	CATCH (!(part_repeat = (PatternBranchPartRepeat *)list_first (branch->parts)->data));
	CATCH (part_repeat->token->from != 0);
	CATCH (part_repeat->token->to != 2);
	CATCH (!(part_a = (PatternBranchPartValue *)part_repeat->part));
	CATCH (part_a->token->value != 'a');
	pattern_branch_destroy (branch);
	pattern_tokens_destroy (tokens);
	CATCH (error_count () != 0);
	PASS ();
}

bool test_pattern_branch_create_8 (Test *test)
{
	List *tokens;
	PatternBranch *branch;
	PatternBranchPartValue *part_a;
	PatternBranchPartValue *part_b;
	PatternBranchPartRepeat *part_repeat;

	TITLE ();
	CATCH (!(tokens = pattern_tokens_create ("ab{2-4}")));
	CATCH (!(branch = pattern_branch_create (tokens)));
	CATCH (!branch->parts);
	CATCH (list_count (branch->parts) != 2);
	CATCH (!(part_a = (PatternBranchPartValue *)list_first (branch->parts)->data));
	CATCH (part_a->token->value != 'a');
	CATCH (!(part_repeat = (PatternBranchPartRepeat *)list_last (branch->parts)->data));
	CATCH (part_repeat->token->from != 2);
	CATCH (part_repeat->token->to != 4);
	CATCH (!(part_b = (PatternBranchPartValue *)part_repeat->part));
	CATCH (part_b->token->value != 'b');
	pattern_branch_destroy (branch);
	pattern_tokens_destroy (tokens);
	CATCH (error_count () != 0);
	PASS ();
}
