#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <lib.core/tainio-tree.h>
#include <lib.core/random.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

static TainioTreeNode *node_insert (TainioTree *tree, TainioTreeNode *root, 
                                    unsigned long long from, 
                                    unsigned long long to, 
                                    unsigned long long *value);
static void node_destroy (TainioTree *tree, TainioTreeNode *node);
unsigned long long node_count (TainioTree *tree, TainioTreeNode *node);
static TainioTreeNode *skew  (TainioTree *tree, TainioTreeNode *root);
static TainioTreeNode *split (TainioTree *tree, TainioTreeNode *root);

TainioTree *tainio_tree_create (unsigned long long from, unsigned long long to)
{
	TainioTree *tree;
 
	if (from > to) {
		error_code (InvalidArgument, 1);
		return NULL;
	}
	if (!unsigned_long_long_add (to, 1, NULL)) {
		error_code (InvalidArgument, 2);
		return NULL;
	}
	if (!(tree = memory_create (sizeof (TainioTree)))) {
		error_code (FunctionCall, 1);
        	return NULL;
	}
	if (!(tree->nil = memory_create (sizeof (TainioTreeNode)))) {
		memory_destroy (tree);
		error_code (FunctionCall, 2);
		return NULL;
	}
	tree->nil->level = 0;
    	tree->nil->value = 0;
    	tree->nil->link[0] = tree->nil->link[1] = tree->nil;
    	tree->root = tree->nil;
	tree->count = 0;
	tree->from = from;
	tree->to = to;
    	return tree;
}

void tainio_tree_destroy (TainioTree *tree)
{
	if (!tree) {
		error (InvalidArgument);
        	return;
	}
	if (tree->root != tree->nil) {
		node_destroy (tree, tree->root);
	}
	memory_destroy (tree->nil);
    	memory_destroy (tree);
}

bool tainio_tree_next (TainioTree *tree, unsigned long long *value)
{
	TainioTreeNode *node;

	if (!tree) {
		error_code (InvalidArgument, 1);
		return false;
	}
	if (!value) {
		error_code (InvalidArgument, 2);
		return false;
	}
	if (tree->count > tree->to - tree->from) {
		error (InvalidOperation);
		return false;
	}
	if (!(node = node_insert (tree, tree->root, tree->from, tree->to, value))) {
		return false;
	}
    	tree->root = node;
	tree->count++;
	return true;
}

static TainioTreeNode *node_insert (TainioTree *tree, TainioTreeNode *root, 
                                    unsigned long long from, 
                                    unsigned long long to, 
                                    unsigned long long *value)
{
	TainioTreeNode *node = NULL;
	int direction;

	if (root == tree->nil) {
		if (!(root = memory_create (sizeof (TainioTreeNode)))) {
			error_code (FunctionCall, 1);
			return NULL;
		}
		if (!random_value (from, to, value)) {
			memory_destroy (root);
			error_code (FunctionCall, 2);
			return NULL;
		}
		root->count = 0;
		root->level = 1;
        	root->value = *value;
		root->link[0] = root->link[1] = tree->nil;
    	}
    	else {
		if (node_count (tree, root->link[0]) == root->value - from &&
		    node_count (tree, root->link[1]) == to - root->value) {
			error (InvalidOperation);
			return NULL;
		}
		else if (node_count (tree, root->link[0]) == root->value - from) {
			direction = 1;
		}
		else if (node_count (tree, root->link[1]) == to - root->value) {
			direction = 0;
		}
		else {
			if (!random_value (1, 
					   ((root->value - from) - node_count (tree, root->link[0])) +
					   ((to - root->value) - node_count (tree, root->link[1])), 
					   value)) {
				error_code (FunctionCall, 3);
				return NULL;
			}
			if (*value > (root->value - from) - node_count (tree, root->link[0])) {
				direction = 1;
			}
			else {
				direction = 0;
			}
		}
		if (!(node = node_insert (tree, 
					  root->link[direction],
					  direction == 0 ? from : root->value + 1,
					  direction == 1 ? to : root->value - 1, 
					  value))) {
			return NULL;
		}
		root->link[direction] = node;
		root->count++;
        	root = skew (tree, root);
        	root = split (tree, root);
    	}
    	return root;
}

static void node_destroy (TainioTree *tree, TainioTreeNode *node)
{
	if (node->link[0] != tree->nil) {
		node_destroy (tree, node->link[0]);
	}
	if (node->link[1] != tree->nil) {
		node_destroy (tree, node->link[1]);
	}
	memory_destroy (node);
}

unsigned long long node_count (TainioTree *tree, TainioTreeNode *node)
{
	return node == tree->nil ? 0 : 1 + node->count;
}

static TainioTreeNode *skew (TainioTree *tree, TainioTreeNode *root)
{
	TainioTreeNode *swap;

    	if (root->link[0]->level == root->level && 
	    root->level != 0) {
		root->count = 
			node_count (tree, root->link[0]->link[1]) + 
			node_count (tree, root->link[1]);
		root->link[0]->count = 
			node_count (tree, root->link[0]->link[0]) + 
			node_count (tree, root);
		swap = root->link[0];
       		root->link[0] = swap->link[1];
        	swap->link[1] = root;
        	root = swap;
    	}
    	return root;
}

static TainioTreeNode *split (TainioTree *tree, TainioTreeNode *root)
{
	TainioTreeNode *swap;

	if (root->link[1]->link[1]->level == root->level && 
	    root->level != 0) {
		root->count = 
			node_count (tree, root->link[1]->link[0]) +
			node_count (tree, root->link[0]);
		root->link[1]->count = 
			node_count (tree, root->link[1]->link[1]) +
			node_count (tree, root);
        	swap = root->link[1];
        	root->link[1] = swap->link[0];
        	swap->link[0] = root;
        	root = swap;
       		root->level++;
    	}
    	return root;
}
