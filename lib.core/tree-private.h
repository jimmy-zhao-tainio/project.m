#ifndef tree_private_h
#define tree_private_h

#include <lib.core/tree.h>

TreeNode *tree_node_insert (Tree *tree, TreeNode *root, Object *key, void *value);
void tree_iterator_path_size (uint8_t size);

#endif
