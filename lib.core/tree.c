#include <stdlib.h>
#include <stdio.h>

#include <lib.core/tree.h>
#include <lib.core/tree-private.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

static TreeNode *node_create  (Tree *tree, Object *key, size_t level);
static void      node_destroy (Tree *tree, TreeNode *node);

static TreeNode *skew  (TreeNode *root);
static TreeNode *split (TreeNode *root);

static uint8_t iterator_path_size = TREE_ITERATOR_PATH_SIZE;

Tree *tree_create ()
{
	Tree *tree;
 
	if (!(tree = memory_create (sizeof (Tree)))) {
		error (FunctionCall);
        	return NULL;
	}
	if (!(tree->nil = memory_create (sizeof (TreeNode)))) {
		memory_destroy (tree);
		error (FunctionCall);
		return NULL;
	}
	tree->nil->level = 0;
	tree->nil->key = NULL;
    	tree->nil->value = NULL;
    	tree->nil->link[0] = tree->nil->link[1] = tree->nil;
    	tree->root = tree->nil;
	tree->count = 0;
    	return tree;
}

void tree_destroy (Tree *tree)
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

bool tree_insert (Tree *tree, Object *key, void *value)
{
	TreeNode *node;

	if (!tree) {
		error (InvalidArgument);
		return false;
	}
	if (!key) {
		error (InvalidArgument);
		return false;
	}
	if (!size_t_add (tree->count, 1, NULL)) {
		error (Overflow);
		return false;
	}
	if (!(node = tree_node_insert (tree, tree->root, key, value))) {
		return false;
	}
    	tree->root = node;
	tree->count++;
	return true;
}

TreeNode *tree_node_insert (Tree *tree, TreeNode *root, Object *key, void *value)
{
	if (root == tree->nil) {
		if (!(root = node_create (tree, key, 1))) {
			error (FunctionCall);
			return NULL;
		}
        	root->value = value;
    	}
    	else {
		TreeNode *node = NULL;
		int direction;

		if (root->key->id == key->id) {
			error (InvalidOperation);
			return NULL;
		}
		direction = root->key->id < key->id;
		if (!(node = tree_node_insert (tree, root->link[direction], key, value))) {
			return NULL;
		}
		root->link[direction] = node;
        	root = skew (root);
        	root = split (root);
    	}
    	return root;
}

void *tree_search (Tree *tree, Object *key)
{
	TreeNode *node;

	if (!tree) {
		error (InvalidArgument);
		return NULL;
	}
	if (!key) {
		error (InvalidArgument);
		return NULL;
	}
	for (node = tree->root; 
	     node != tree->nil; 
	     node = node->link[node->key->id < key->id]) {
		if (node->key->id == key->id) {
            		return node->value;
		}
    	}
	return NULL;
}

void tree_iterator_path_size (uint8_t size)
{
	if (size == 0) {
		size = TREE_ITERATOR_PATH_SIZE;
	}
	iterator_path_size = size;
}

TreeIterator *tree_iterator_create (Tree *tree)
{
	TreeIterator *iterator;

	if (!(iterator = memory_create (sizeof (TreeIterator)))) {
		error (FunctionCall);
		return NULL;
	}
	if (!(iterator->path = memory_create (sizeof (TreeNode *) * iterator_path_size))) {
		memory_destroy (iterator);
		error (FunctionCall);
		return NULL;
	}
	iterator->tree = tree;
	iterator->index = 0;
	iterator->index_is_set = false;
	iterator->node = NULL;
	iterator->key = NULL;
	iterator->value = NULL;
	return iterator;
}

bool tree_iterator_next (TreeIterator *iterator)
{
	if (!iterator) {
		error (InvalidArgument);
		return false;
	}
	iterator->key = NULL;
	iterator->value = NULL;
	if (iterator->node == NULL) {
		iterator->node = iterator->tree->root;
		iterator->index = 0;
		iterator->index_is_set = false;
	}
	while (iterator->index_is_set != false || 
	       iterator->node != iterator->tree->nil) {
		if (iterator->node != iterator->tree->nil) {
			if (iterator->index_is_set == false) {
				iterator->index_is_set = true;
			}
			else {
				iterator->index++;
				if (iterator->index >= iterator_path_size) {
					error (Overflow);
					return false;
				}
			}
			iterator->path[iterator->index] = iterator->node;
			iterator->node = iterator->node->link[0];
		}
		else {
			iterator->node = iterator->path[iterator->index];
			if (iterator->index == 0) {
				iterator->index_is_set = false;
			}
			else {
				iterator->index--;
			}
			iterator->key = iterator->node->key;
			iterator->value = iterator->node->value;
			iterator->node = iterator->node->link[1];
			return true;
		}
	}
	return false;
}

void tree_iterator_destroy (TreeIterator *iterator)
{
	if (!iterator) {
		error (InvalidArgument);
		return;
	}
	memory_destroy (iterator->path);
	memory_destroy (iterator);
}

size_t tree_count (Tree *tree)
{
	if (!tree) {
		error (InvalidArgument);
		return 0;
	}
	return tree->count;
}

static TreeNode *node_create (Tree *tree, Object *key, size_t level)
{
	TreeNode *node;

	if (!(node = memory_create (sizeof (TreeNode)))) {
		error (FunctionCall);
        	return NULL;
	}
    	node->key = key;
    	node->level = level;
    	node->link[0] = node->link[1] = tree->nil;
    	return node;
}

static void node_destroy (Tree *tree, TreeNode *node)
{
	if (node->link[0] != tree->nil) {
		node_destroy (tree, node->link[0]);
	}
	if (node->link[1] != tree->nil) {
		node_destroy (tree, node->link[1]);
	}
	memory_destroy (node);
}

static TreeNode *skew (TreeNode *root)
{
    	if (root->link[0]->level == root->level && 
	    root->level != 0) {
        	TreeNode *save = root->link[0];
       		root->link[0] = save->link[1];
        	save->link[1] = root;
        	root = save;
    	}
    	return root;
}

static TreeNode *split (TreeNode *root)
{
	if (root->link[1]->link[1]->level == root->level && 
	    root->level != 0) {
        	TreeNode *save = root->link[1];
        	root->link[1] = save->link[0];
        	save->link[0] = root;
        	root = save;
       		root->level++;
    	}
    	return root;
}
