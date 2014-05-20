#include <stdlib.h>
#include <stdio.h>

#include <lib.core/tainio-tree.h>
#include <lib.core/permute.h>
#include <lib.core/memory.h>
#include <lib.core/random.h>
#include <lib.core/error.h>
#include <lib.core/defines-private.h>

#include "test-tainio-tree.h"

static bool validate_tainio_tree                (TainioTree *tree);
static bool validate_tainio_tree_bst            (TainioTree *tree, TainioTreeNode *node, TainioTreeNode **previous);
static bool validate_tainio_tree_aa_rule_1      (TainioTree *tree, TainioTreeNode *node);
static bool validate_tainio_tree_aa_rule_2      (TainioTree *tree, TainioTreeNode *node);
static bool validate_tainio_tree_aa_rule_3      (TainioTree *tree, TainioTreeNode *node);
static bool validate_tainio_tree_aa_rule_4      (TainioTree *tree, TainioTreeNode *node);
static bool validate_tainio_tree_aa_rule_5      (TainioTree *tree, TainioTreeNode *node);
static bool validate_tainio_tree_aa_balance     (TainioTree *tree);
static bool validate_tainio_tree_aa_balance_min (TainioTree *tree, TainioTreeNode *node);
static bool validate_tainio_tree_aa_balance_max (TainioTree *tree, TainioTreeNode *node);
static bool validate_tainio_tree_node_count     (TainioTree *tree, TainioTreeNode *node);

bool test_tainio_tree_create_invalid_argument_1 (Test *test)
{
	TITLE ();
	CATCH (tainio_tree_create (2, 1));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_tainio_tree_create_invalid_argument_2 (Test *test)
{
	TITLE ();
	CATCH (tainio_tree_create (0, (unsigned long long)-1));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 2);
	PASS ();
}

bool test_tainio_tree_create_function_call_1 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (TainioTree) - 1);
	CATCH (tainio_tree_create (0, 1));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_tainio_tree_create_function_call_2 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (TainioTree) + sizeof (TainioTreeNode) - 1);
	CATCH (tainio_tree_create (0, 1));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 2);
	PASS ();
}

bool test_tainio_tree_create (Test *test)
{
	TainioTree *tree;

	TITLE ();
	CATCH (!(tree = tainio_tree_create (0, 1)));
	tainio_tree_destroy (tree);
	PASS ();
}

bool test_tainio_tree_destroy_invalid_argument (Test *test)
{
	TITLE ();
	tainio_tree_destroy (NULL);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_tainio_tree_next_invalid_argument_1 (Test *test)
{
	unsigned long long value;

	TITLE ();
	CATCH (tainio_tree_next (NULL, &value));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_tainio_tree_next_invalid_argument_2 (Test *test)
{
	TainioTree *tree;

	TITLE ();
	CATCH (!(tree = tainio_tree_create (0, 1)));
	CATCH (tainio_tree_next (tree, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 2);
	tainio_tree_destroy (tree);
	PASS ();
}

bool test_tainio_tree_next_invalid_operation (Test *test)
{
	TainioTree *tree;
	unsigned long long value;

	TITLE ();
	CATCH (!random_open ());
	CATCH (!(tree = tainio_tree_create (0, 0)));
	CATCH (!tainio_tree_next (tree, &value));
	CATCH (tainio_tree_next (tree, &value));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	tainio_tree_destroy (tree);
	random_close ();
	PASS ();
}

bool test_tainio_tree_next_overflow (Test *test)
{
	TainioTree *tree;
	unsigned long long value;

	TITLE ();
	CATCH (!random_open ());
	CATCH (!(tree = tainio_tree_create (0, 1)));
	unsigned_long_long_private_max (0);
	CATCH (tainio_tree_next (tree, &value));
	CATCH (error_count () != 2);
	CATCH (error_at (1).error != ErrorOverflow);
	tainio_tree_destroy (tree);
	random_close ();
	PASS ();
}

bool test_tainio_tree_next_function_call_1 (Test *test)
{
	TainioTree *tree;
	unsigned long long value;

	TITLE ();
	CATCH (!(tree = tainio_tree_create (0, 1)));
	memory_commit_limit (memory_commit_size ());
	CATCH (tainio_tree_next (tree, &value));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 1);
	tainio_tree_destroy (tree);
	PASS ();
}

bool test_tainio_tree_next_function_call_2 (Test *test)
{
	TainioTree *tree;
	unsigned long long value;

	TITLE ();
	CATCH (!(tree = tainio_tree_create (0, 1)));
	CATCH (tainio_tree_next (tree, &value));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 2);
	tainio_tree_destroy (tree);
	PASS ();
}

bool test_tainio_tree_next_function_call_3 (Test *test)
{
	TainioTree *tree;
	unsigned long long value;

	TITLE ();
	CATCH (!random_open ());
	CATCH (!(tree = tainio_tree_create (0, (unsigned long long)-2)));
	CATCH (!tainio_tree_next (tree, &value));
	random_close ();
	CATCH (tainio_tree_next (tree, &value));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 3);
	tainio_tree_destroy (tree);
	PASS ();
}

bool test_tainio_tree_next_function_call_1_2 (Test *test)
{
	TainioTree *tree;
	unsigned long long value;

	TITLE ();
	CATCH (!random_open ());
	CATCH (!(tree = tainio_tree_create (0, 1)));
	CATCH (!tainio_tree_next (tree, &value));
	memory_commit_limit (memory_commit_size ());
	CATCH (tainio_tree_next (tree, &value));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 1);
	tainio_tree_destroy (tree);
	random_close ();
	PASS ();
}

bool test_tainio_tree_next (Test *test)
{
	TainioTree *tree;
	unsigned long long value;
	size_t i;

	TITLE ();
	CATCH (!random_open ());
	CATCH (!(tree = tainio_tree_create (0, 999)));
	for (i = 0; i < 1000; i++) {
		CATCH (!tainio_tree_next (tree, &value));
		CATCH (!validate_tainio_tree (tree));
	}
	CATCH (tainio_tree_next (tree, &value));
	tainio_tree_destroy (tree);
	random_close ();
	PASS ();
}

static bool validate_tainio_tree (TainioTree *tree)
{
	TainioTreeNode *previous = NULL;

	if (!validate_tainio_tree_bst (tree, tree->root, &previous) ||
	    !validate_tainio_tree_aa_rule_1 (tree, tree->root) ||
	    !validate_tainio_tree_aa_rule_2 (tree, tree->root) ||
	    !validate_tainio_tree_aa_rule_3 (tree, tree->root) ||
	    !validate_tainio_tree_aa_rule_4 (tree, tree->root) ||
	    !validate_tainio_tree_aa_rule_5 (tree, tree->root) ||
	    !validate_tainio_tree_aa_balance (tree) ||
	    !validate_tainio_tree_node_count (tree, tree->root)) {
		return false;
	}
	return true;
}

static bool validate_tainio_tree_bst (TainioTree *tree, TainioTreeNode *node, TainioTreeNode **previous)
{
	if (node == tree->nil) {
		return true;
	}
	if (!validate_tainio_tree_bst (tree, node->link[0], previous)) {
		error (InvalidOperation);
		return false;
	}
	if (*previous != NULL && node->value < (*previous)->value) {
		error (InvalidOperation);
		return false;
	}
	*previous = node;
	if (!validate_tainio_tree_bst (tree, node->link[1], previous)) {
		error (InvalidOperation);
		return false;
	}
	return true;
}

static bool validate_tainio_tree_aa_rule_1 (TainioTree *tree, TainioTreeNode *node)
{
	if (node == tree->nil) {
		return true;
	}
	/* Leaf nodes are at level one. */
	if (node->link[0] == tree->nil &&
		node->link[1] == tree->nil) {
		if (node->level == 1) {
			return true;
		}
		else {
			return false;
		}
	}
	if (node->link[0] != tree->nil &&
		node->link[1] != tree->nil) {
		if (!validate_tainio_tree_aa_rule_1 (tree, node->link[0])) {
			return false;
		}
		return validate_tainio_tree_aa_rule_1 (tree, node->link[1]);
	}
	if (node->link[0] != tree->nil) {
		return validate_tainio_tree_aa_rule_1 (tree, node->link[0]);
	}
	else {
		return validate_tainio_tree_aa_rule_1 (tree, node->link[1]);
	}
}

static bool validate_tainio_tree_aa_rule_2 (TainioTree *tree, TainioTreeNode *node)
{
	/* Left children must have a level less than their parent. */
	if (node == tree->nil) {
		return true;
	}
	if (node->level != node->link[0]->level + 1) {
		return false;
	}
	if (!validate_tainio_tree_aa_rule_2 (tree, node->link[0])) {
		return false;
	}
	return validate_tainio_tree_aa_rule_2 (tree, node->link[1]);
}

static bool validate_tainio_tree_aa_rule_3 (TainioTree *tree, TainioTreeNode *node)
{
	/* Right children must have a level less than or equal to their parent. */
	if (node == tree->nil) {
		return true;
	}
	if (node->level != node->link[1]->level &&
	    node->level != node->link[1]->level + 1) {
		return false;
	}
	if (!validate_tainio_tree_aa_rule_3 (tree, node->link[0])) {
		return false;
	}
	return validate_tainio_tree_aa_rule_3 (tree, node->link[1]);
}

static bool validate_tainio_tree_aa_rule_4 (TainioTree *tree, TainioTreeNode *node)
{
	/* Right grandchildren must have a level less than their grandparent. */
	if (node == tree->nil) {
		return true;
	}
	if (node->level == node->link[1]->level) {
		/* GP:X P:X GC:X-1 */
		if (node->link[1]->level != node->link[1]->link[1]->level + 1) {
			return false;
		}
	}
	else if (node->level == node->link[1]->level + 1) {
		/* GP:X P:X-1 GC:X-1 */
		/* GP:X P:X-1 GC:X-2 */
		if (node->link[1]->level != node->link[1]->link[1]->level &&
		    node->link[1]->level != node->link[1]->link[1]->level + 1) {
			return false;
		}
	}
	else {
		/* GP:? P:? GC:? */
		return false;
	}
	if (!validate_tainio_tree_aa_rule_4 (tree, node->link[0])) {
		return false;
	}
	return validate_tainio_tree_aa_rule_4 (tree, node->link[1]);
}

static bool validate_tainio_tree_aa_rule_5 (TainioTree *tree, TainioTreeNode *node)
{
	/* Any node with a level higher than one has to have two children. */
	if (node == tree->nil) {
		return true;
	}
	if (node->level > 1) {
		if (node->link[0] == tree->nil &&
		    node->link[1] == tree->nil) {
			return false;
		}
	}
	if (!validate_tainio_tree_aa_rule_5 (tree, node->link[0])) {
		return false;
	}
	return validate_tainio_tree_aa_rule_5 (tree, node->link[1]);
}

static bool validate_tainio_tree_aa_balance (TainioTree *tree)
{
	size_t min;
	size_t max;

	min = validate_tainio_tree_aa_balance_min (tree, tree->root);
	max = validate_tainio_tree_aa_balance_max (tree, tree->root);
	if (min > max) {
		return false;
	}
	if (max - min <= 1) {
		return true;
	}
	return false;
}

static bool validate_tainio_tree_aa_balance_max (TainioTree *tree, TainioTreeNode *node)
{
	size_t left;
	size_t right;
	size_t right_pseudo = 0;

	if (node == tree->nil) {
		return false;
	}
	left = validate_tainio_tree_aa_balance_max (tree, node->link[0]);
	right = validate_tainio_tree_aa_balance_max (tree, node->link[1]);
	if (node->link[1]->level == node->level) {
		right_pseudo = 1;
	}
	if (left > right) {
		return 1 + left;
	}
	return (right_pseudo ? 0 : 1) + right;
}

static bool validate_tainio_tree_aa_balance_min (TainioTree *tree, TainioTreeNode *node)
{
	size_t left;
	size_t right;
	size_t right_pseudo = 0;

	if (node == tree->nil) {
		return false;
	}
	left = validate_tainio_tree_aa_balance_min (tree, node->link[0]);
	right = validate_tainio_tree_aa_balance_min (tree, node->link[1]);
	if (node->link[1]->level == node->level) {
		right_pseudo = 1;
	}
	if (left < right) {
		return 1 + left;
	}
	return (right_pseudo ? 0 : 1) + right;
}

static bool validate_tainio_tree_node_count (TainioTree *tree, TainioTreeNode *node)
{
	if (node == tree->nil) {
		if (node->count != 0) {
			error (InvalidOperation);
			return false;
		}
		return true;
	}
	if (!validate_tainio_tree_node_count (tree, node->link[0]) ||
	    !validate_tainio_tree_node_count (tree, node->link[1])) {
		error (InvalidOperation);
		return false;
	}
	if (node->count != 
	    (node->link[0] == tree->nil ? 0 : 1 + node->link[0]->count) + 
	    (node->link[1] == tree->nil ? 0 : 1 + node->link[1]->count)) {
		error (InvalidOperation);
		return false;
	}
	if (node->link[0] == tree->nil &&
	    node->link[1] == tree->nil) {
		if (node->count != 0) {
			error (InvalidOperation);
			return false;
		}
		return true;
	}
	if (node->link[0] != tree->nil &&
	    node->link[1] == tree->nil &&
	    node->link[0]->link[0] == tree->nil &&
	    node->link[0]->link[1] == tree->nil) {
		if (node->count != 1) {
			error (InvalidOperation);
			return false;
		}
		return true;
	}
	if (node->link[0] == tree->nil &&
	    node->link[1] != tree->nil &&
	    node->link[1]->link[0] == tree->nil &&
	    node->link[1]->link[1] == tree->nil) {
	    	if (node->count != 1) {
			error (InvalidOperation);
			return false;
		}
		return true;
	}
	if (node->link[0] != tree->nil &&
	    node->link[1] != tree->nil &&
	    node->link[0]->link[0] == tree->nil &&
	    node->link[0]->link[1] == tree->nil &&
	    node->link[1]->link[0] == tree->nil &&
	    node->link[1]->link[1] == tree->nil) {
		if (node->count != 2) {
			error (InvalidOperation);
			return false;
		}
		return true;
	}
	return true;
}
