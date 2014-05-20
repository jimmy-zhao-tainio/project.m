#ifndef tree_h
#define tree_h

#include <lib.core/defines.h>
#include <lib.core/object.h>

#define TREE_ITERATOR_PATH_SIZE 32

typedef struct _Tree         Tree;
typedef struct _TreeNode     TreeNode;
typedef struct _TreeIterator TreeIterator;

struct _Tree
{
   	TreeNode *root;
	TreeNode *nil;
	size_t count;
};

struct _TreeNode
{
	size_t level;
	TreeNode *link[2];
	Object *key;
	void *value;
};

struct _TreeIterator
{
	Tree *tree;
	TreeNode **path;	
	size_t index;
	bool index_is_set;
	TreeNode *node;
	Object *key;
	void *value;
};

Tree *tree_create (void);
void  tree_destroy (Tree *tree);

bool  tree_insert (Tree *tree, Object *key, void *value);
void *tree_search (Tree *tree, Object *key);

TreeIterator *tree_iterator_create  (Tree *tree);
bool          tree_iterator_next    (TreeIterator *iterator);
void          tree_iterator_destroy (TreeIterator *iterator);

size_t tree_count (Tree *tree);

#endif
