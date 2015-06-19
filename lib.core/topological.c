#include <stdlib.h>

#include <lib.core/defines.h>
#include <lib.core/memory.h>
#include <lib.core/topological.h>
#include <lib.core/error.h>

static void             topological_sort_reset   (Topological *topological);
static TopologicalNode *topological_node_create  (Topological *topological, Object *data);
static void             topological_node_destroy (TopologicalNode *node);

Topological *topological_create (void)
{
	Topological *topological;

	if (!(topological = memory_create (sizeof (Topological)))) {
		error_code (FunctionCall, 1);
		return NULL;
	}
	if (!(topological->data_to_node = tree_create ())) {
		memory_destroy (topological);
		error_code (FunctionCall, 2);
		return NULL;
	}
	if (!(topological->nodes = list_create ())) {
		tree_destroy (topological->data_to_node);
		memory_destroy (topological);
		error_code (FunctionCall, 3);
		return NULL;
	}
	return topological;
}

void topological_destroy (Topological *topological)
{
	ListNode *node;

	if (!topological) {
		error (InvalidArgument);
		return;
	}
	if (topological->data_to_node) {
		tree_destroy (topological->data_to_node);
	}
	if (topological->nodes) {
		for (node = list_first (topological->nodes); 
                     node; 
                     node = list_next (node)) {
			topological_node_destroy (node->data);
		}
		list_destroy (topological->nodes);
	}
	memory_destroy (topological);
}

bool topological_add_vertex (Topological *topological, Object *u)
{
	if (!topological) {
		error_code (InvalidArgument, 1);
		return false;
	}
	if (!u) {
		error_code (InvalidArgument, 2);
		return false;
	}
	if (tree_search (topological->data_to_node, u)) {
		error (InvalidOperation);
		return false;
	}
	if (!topological_node_create (topological, u)) {
		error (FunctionCall);
		return false;
	}
	return true;
}

bool topological_contains_vertex (Topological *topological, Object *u)
{
	if (!topological) {
		error_code (InvalidArgument, 1);
		return false;
	}
	if (!u) {
		error_code (InvalidArgument, 2);
		return false;
	}
	if (tree_search (topological->data_to_node, u)) {
		return true;
	}
	return false;
}

bool topological_set_edge (Topological *topological, Object *u, Object *v)
{
	TopologicalNode *u_node;
	TopologicalNode *v_node;

	if (!topological) {
		error_code (InvalidArgument, 1);
		return false;
	}
	if (!u) {
		error_code (InvalidArgument, 2);
		return false;
	}
	if (!v) {
		error_code (InvalidArgument, 3);
		return false;
	}
	if (!(u_node = tree_search (topological->data_to_node, u))) {
		error_code (InvalidOperation, 1);
		return false;
	}
	if (!(v_node = tree_search (topological->data_to_node, v))) {
		error_code (InvalidOperation, 2);
		return false;
	}
	if (!list_append (u_node->nodes, v_node)) {
		error (FunctionCall);
		return false;
	}
	if (!size_t_add (v_node->degree, 1, &v_node->degree)) {
		error_code (Overflow, 1);
		return false;
	}
	if (!size_t_add (v_node->degree_copy, 1, &v_node->degree_copy)) {
		v_node->degree--;
		error_code (Overflow, 2);
		return false;
	}
	if (v_node->degree == 1) {
		list_position (topological->nodes, 
                               list_last (topological->nodes), 
                               v_node->list_node);
	}
	return true;
}

List *topological_sort (Topological *topological)
{
	List *list;
	ListNode *list_node;
	ListNode *list_node_dependency;
	TopologicalNode *node;
	TopologicalNode *node_dependency;

	if (!topological) {
		error (InvalidArgument);
		return NULL;
	}
	if (!(list = list_create ())) {
		error_code (FunctionCall, 1);
		return NULL;
	}
	for (list_node = list_first (topological->nodes); 
	     list_node; 
	     list_node = list_next (list_node)) {
		node = list_node->data;
		if (node->degree != 0) {
			list_destroy (list);
			topological_sort_reset (topological);
			error_code (InvalidOperation, 1);
			return NULL;
		}
		for (list_node_dependency = list_first (node->nodes); 
		     list_node_dependency; 
		     list_node_dependency = list_next (list_node_dependency)) {
			node_dependency = list_node_dependency->data;
			if (node_dependency->degree == 0) {
				list_destroy (list);
				topological_sort_reset (topological);
				error_code (InvalidOperation, 2);
				return NULL;
			}
			if (--node_dependency->degree == 0) {
				list_position (topological->nodes, list_node, node_dependency->list_node);
			}
		}
		if (!list_append (list, node->data)) {
			list_destroy (list);
			topological_sort_reset (topological);
			error_code (FunctionCall, 2);
			return NULL;
		}
	}
	topological_sort_reset (topological);
	return list;
}

List *topological_get_edges (Topological *topological, Object *u)
{
	TopologicalNode *node;
	List *list;
	ListNode *list_node;

	if (!topological) {
		error_code (InvalidArgument, 1);
		return NULL;
	}
	if (!u) {
		error_code (InvalidArgument, 2);
		return NULL;
	}
	if (!(node = tree_search (topological->data_to_node, u))) {
		error (InvalidOperation);
		return NULL;
	}
	if (!(list = list_create ())) {
		error_code (FunctionCall, 1);
		return NULL;
	}
	for (list_node = list_first (node->nodes); 
             list_node; 
             list_node = list_next (list_node)) {
		if (!list_append (list, ((TopologicalNode *)list_node->data)->data)) {
			list_destroy (list);
			error_code (FunctionCall, 2);
			return NULL;
		}
	}
	return list;
}

size_t topological_count_vertices (Topological *topological)
{
	if (!topological) {
		error (InvalidArgument);
		return 0;
	}
	return list_count (topological->nodes);
}

static void topological_sort_reset (Topological *topological)
{
	ListNode *node;

	for (node = list_first (topological->nodes); 
             node; 
             node = list_next (node)) {
		((TopologicalNode *)node->data)->degree = ((TopologicalNode *)node->data)->degree_copy;
	}
}

static TopologicalNode *topological_node_create (Topological *topological, Object *data)
{
	TopologicalNode *node;

	if (!(node = memory_create (sizeof (TopologicalNode)))) {
		error (FunctionCall);
		return NULL;
	}
	if (!(node->nodes = list_create ())) {
		memory_destroy (node);
		error (FunctionCall);
		return NULL;
	}
	node->degree = 0;
	node->degree_copy = 0;
	node->data = data;
	if (!(node->list_node = list_insert (topological->nodes, node))) {
		list_destroy (node->nodes);
		memory_destroy (node);
		error (FunctionCall);
		return NULL;
	}
	if (!tree_insert (topological->data_to_node, data, node)) {
		list_remove (topological->nodes, node->list_node);
		list_destroy (node->nodes);
		memory_destroy (node);
		error (FunctionCall);
		return NULL;
	}
	return node;
}

static void topological_node_destroy (TopologicalNode *node)
{
	if (!node) {
		error (InvalidArgument);
		return;
	}
	if (node->nodes) {
		list_destroy (node->nodes);
	}
	memory_destroy (node);
}

