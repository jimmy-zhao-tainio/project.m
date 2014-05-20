#ifndef tainio_tree_h
#define tainio_tree_h

#include <lib.core/defines.h>

typedef struct _TainioTree         TainioTree;
typedef struct _TainioTreeNode     TainioTreeNode;

struct _TainioTree
{
   	TainioTreeNode *root;
	TainioTreeNode *nil;
	unsigned long long count;
	unsigned long long from;
	unsigned long long to;
};

struct _TainioTreeNode
{
	TainioTreeNode *link[2];
	unsigned long long count;
	unsigned long long level;
	unsigned long long value;
};

TainioTree *tainio_tree_create (unsigned long long from, unsigned long long to);
void        tainio_tree_destroy (TainioTree *tree);

bool tainio_tree_next (TainioTree *tree, unsigned long long *value);

#endif
