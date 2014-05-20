#ifndef topological_h
#define topological_h

#include <lib.core/object.h>
#include <lib.core/list.h>
#include <lib.core/tree.h>

typedef struct s_Topological Topological;
typedef struct s_TopologicalNode TopologicalNode;

struct s_Topological
{
	Tree *data_to_node;
	List *nodes;
};

struct s_TopologicalNode
{
	List *nodes;
	ListNode *list_node;
	size_t degree;
	size_t degree_copy;
	Object *data;
};

Topological *topological_create  (void);
void         topological_destroy (Topological *topological);

bool   topological_add_vertex      (Topological *topological, Object *u);
bool   topological_contains_vertex (Topological *topological, Object *u);
bool   topological_set_edge        (Topological *topological, Object *u, Object *v);
List  *topological_get_edges       (Topological *topological, Object *u);
List  *topological_sort            (Topological *topological);
size_t topological_count_vertices  (Topological *topological);

#endif
