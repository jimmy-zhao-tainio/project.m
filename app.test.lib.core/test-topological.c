#include <stdlib.h>
#include <stdio.h>
#include <lib.core/topological.h>
#include <lib.core/string.h>
#include <lib.core/random.h>
#include <lib.core/tainio-tree.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.core/defines-private.h>

#include "test-topological.h"

static size_t shuffled[1000]; 
static bool shuffled_set (void);

bool test_topological_create_function_call_1 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (Topological) - 1);
	CATCH (topological_create ());
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_topological_create_function_call_2 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (Topological) +
			     sizeof (size_t) + sizeof (Tree) + 
			     sizeof (size_t) + sizeof (TreeNode) - 1);
	CATCH (topological_create ());
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 2);
	PASS ();
}

bool test_topological_create_function_call_3 (Test *test)
{
	TITLE ();
	memory_commit_limit (sizeof (size_t) + sizeof (Topological) +
			     sizeof (size_t) + sizeof (Tree) + 
			     sizeof (size_t) + sizeof (TreeNode));
	CATCH (topological_create ());
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 3);
	PASS ();
}

bool test_topological_create (Test *test)
{
	Topological *topological;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (topological_count_vertices (topological) != 0);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_destroy_invalid_argument (Test *test)
{
	TITLE ();
	topological_destroy (NULL);
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_topological_destroy (Test *test)
{
	TITLE ();
	topological_destroy (topological_create ());
	PASS ();
}

bool test_topological_add_vertex_invalid_argument_1 (Test *test)
{
	TITLE ();
	CATCH (topological_add_vertex (NULL, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_topological_add_vertex_invalid_argument_2 (Test *test)
{
	Topological *topological;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (topological_add_vertex (topological, NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 2);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_add_vertex_invalid_operation (Test *test)
{
	Topological *topological;
	Object vertex = { 0 };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertex));
	CATCH (!topological_contains_vertex (topological, &vertex));
	CATCH (topological_add_vertex (topological, &vertex));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (topological_count_vertices (topological) != 1);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_add_vertex_function_call (Test *test)
{
	Topological *topological;
	Object vertex = { 0 };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	memory_commit_limit (memory_commit_size ());
	CATCH (topological_add_vertex (topological, &vertex));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_add_vertex (Test *test)
{
	Topological *topological;
	Object vertex = { 0 };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertex));
	CATCH (!topological_contains_vertex (topological, &vertex));
	CATCH (topological_count_vertices (topological) != 1);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_contains_vertex_invalid_argument_1 (Test *test)
{
	TITLE ();
	CATCH (topological_contains_vertex (NULL, NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_topological_contains_vertex_invalid_argument_2 (Test *test)
{
	Topological *topological;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (topological_contains_vertex (topological, NULL));
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 2);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_contains_vertex (Test *test)
{
	Topological *topological;
	Object vertex = { 0 };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (topological_contains_vertex (topological, &vertex));
	CATCH (!topological_add_vertex (topological, &vertex));
	CATCH (!topological_contains_vertex (topological, &vertex));
	topological_destroy (topological);
	PASS ();
}

bool test_topological_set_edge_invalid_argument_1 (Test *test)
{
	TITLE ();
	CATCH (topological_set_edge (NULL, NULL, NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_topological_set_edge_invalid_argument_2 (Test *test)
{
	Topological *topological;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (topological_set_edge (topological, NULL, NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 2);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_set_edge_invalid_argument_3 (Test *test)
{
	Topological *topological;
	Object u;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (topological_set_edge (topological, &u, NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 3);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_set_edge_invalid_operation_1 (Test *test)
{
	Topological *topological;
	Object u = { 0 };
	Object v = { 1 };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &v));
	CATCH (topological_set_edge (topological, &u, &v));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (error_at (0).code != 1);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_set_edge_invalid_operation_2 (Test *test)
{
	Topological *topological;
	Object u = { 0 };
	Object v = { 1 };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &u));
	CATCH (topological_set_edge (topological, &u, &v));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	CATCH (error_at (0).code != 2);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_set_edge_function_call (Test *test)
{
	Topological *topological;
	Object u = { 0 };
	Object v = { 1 };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &u));
	CATCH (!topological_add_vertex (topological, &v));
	memory_commit_limit (memory_commit_size ());
	CATCH (topological_set_edge (topological, &u, &v));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_set_edge_1 (Test *test)
{
	Topological *topological;
	Object vertex = { 0 };
	List *edges;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertex));
	CATCH (!topological_set_edge (topological, &vertex, &vertex));
	CATCH (!(edges = topological_get_edges (topological, &vertex)));
	CATCH (list_count (edges) != 1);
	CATCH (list_first (edges)->data != &vertex);
	list_destroy (edges);
	CATCH (topological_count_vertices (topological) != 1);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_set_edge_2 (Test *test)
{
	Topological *topological;
	Object u = { 0 };
	Object v = { 1 };
	List *edges;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &u));
	CATCH (!topological_add_vertex (topological, &v));
	CATCH (!topological_set_edge (topological, &u, &v));
	CATCH (!topological_set_edge (topological, &u, &v));
	CATCH (!(edges = topological_get_edges (topological, &u)));
	CATCH (list_count (edges) != 2);
	CATCH (list_first (edges)->data != &v);
	CATCH (list_last (edges)->data != &v);
	list_destroy (edges);
	CATCH (topological_count_vertices (topological) != 2);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_set_edge_3 (Test *test)
{
	Topological *topological;
	Object u = { 0 };
	Object v = { 1 };
	List *edges;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &u));
	CATCH (!topological_add_vertex (topological, &v));
	CATCH (!topological_set_edge (topological, &u, &v));
	CATCH (!(edges = topological_get_edges (topological, &u)));
	CATCH (!list_find (edges, &v));
	list_destroy (edges);
	CATCH (topological_count_vertices (topological) != 2);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_set_edge_4 (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 1 }, { 2 }, { 3 }, { 4 }, { 5 } };
	size_t n1, n2;
	List *edges;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	for (n1 = 0; n1 < 5; n1++) {
		CATCH (!topological_add_vertex (topological, &vertices[n1]));
	}
	for (n1 = 0; n1 < 5; n1++) {
		for (n2 = 0; n2 < 5; n2++) {
			CATCH (!topological_set_edge (topological, &vertices[n1], &vertices[n2]));
		}
	}
	for (n1 = 0; n1 < 5; n1++) {
		CATCH (!(edges = topological_get_edges (topological, &vertices[n1])));
		CATCH (list_count (edges) != 5);
		for (n2 = 0; n2 < 5; n2++) {
			CATCH (!list_find (edges, &vertices[n2]));
		}
		list_destroy (edges);
	}
	CATCH (topological_count_vertices (topological) != 5);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (topological_sort (NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_topological_sort_function_call_1 (Test *test)
{
	Topological *topological;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	memory_commit_limit (0);
	CATCH (topological_sort (topological));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 1);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_function_call_2 (Test *test)
{
	Topological *topological;
	Object u = { 0 };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &u));
	memory_commit_limit (memory_commit_size () + sizeof (size_t) + sizeof (List));
	CATCH (topological_sort (topological));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 2);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_1 (Test *test)
{
	Topological *topological;
	List *sorted;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!(sorted = topological_sort (topological)));
	CATCH (list_count (sorted) != 0);
	list_destroy (sorted);
	CATCH (topological_count_vertices (topological) != 0);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_2 (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 0 } };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertices[0]));
	CATCH (!topological_set_edge (topological, &vertices[0], &vertices[0]));
	CATCH (topological_sort (topological));
	CATCH (topological_count_vertices (topological) != 1);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_3 (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 0 }, { 1 } };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertices[0]));
	CATCH (!topological_add_vertex (topological, &vertices[1]));
	CATCH (!topological_set_edge (topological, &vertices[0], &vertices[1]));
	CATCH (!topological_set_edge (topological, &vertices[1], &vertices[0]));
	CATCH (topological_sort (topological));
	CATCH (topological_count_vertices (topological) != 2);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_4 (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 0 }, { 1 }, { 2 } };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertices[0]));
	CATCH (!topological_add_vertex (topological, &vertices[1]));
	CATCH (!topological_add_vertex (topological, &vertices[2]));
	CATCH (!topological_set_edge (topological, &vertices[0], &vertices[1]));
	CATCH (!topological_set_edge (topological, &vertices[1], &vertices[2]));
	CATCH (!topological_set_edge (topological, &vertices[2], &vertices[0]));
	CATCH (topological_sort (topological));
	CATCH (topological_count_vertices (topological) != 3);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_5 (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 0 }, { 1 }, { 2 }, { 3 }, { 4 } };
	size_t i;
	
	TITLE ();
	CATCH (!(topological = topological_create ()));
	for (i = 0; i < 5; i++) {
		CATCH (!topological_add_vertex (topological, &vertices[i]));
	}
	CATCH (!topological_set_edge (topological, &vertices[0], &vertices[1]));
	CATCH (!topological_set_edge (topological, &vertices[1], &vertices[2]));
	CATCH (!topological_set_edge (topological, &vertices[2], &vertices[3]));
	CATCH (!topological_set_edge (topological, &vertices[3], &vertices[1]));
	CATCH (!topological_set_edge (topological, &vertices[3], &vertices[4]));
	CATCH (topological_sort (topological));
	CATCH (topological_count_vertices (topological) != 5);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_6 (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 0 } };
	List *sorted;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertices[0]));
	CATCH (!(sorted = topological_sort (topological)));
	CATCH (list_count (sorted) != 1);
	CATCH (list_first (sorted)->data != &vertices[0]);
	list_destroy (sorted);
	CATCH (topological_count_vertices (topological) != 1);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_7 (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 0 }, { 1 } };
	List *sorted;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertices[0]));
	CATCH (!topological_add_vertex (topological, &vertices[1]));
	CATCH (!topological_set_edge (topological, &vertices[0], &vertices[1]));
	CATCH (!(sorted = topological_sort (topological)));
	CATCH (list_count (sorted) != 2);
	CATCH (list_first (sorted)->data != &vertices[0]);
	CATCH (list_last (sorted)->data != &vertices[1]);
	list_destroy (sorted);
	CATCH (topological_count_vertices (topological) != 2);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_8 (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 0 }, { 1 }, { 2 } };
	List *sorted;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertices[0]));
	CATCH (!topological_add_vertex (topological, &vertices[1]));
	CATCH (!topological_add_vertex (topological, &vertices[2]));
	CATCH (!topological_set_edge (topological, &vertices[0], &vertices[1]));
	CATCH (!topological_set_edge (topological, &vertices[1], &vertices[2]));
	CATCH (!(sorted = topological_sort (topological)));
	CATCH (list_count (sorted) != 3);
	CATCH (list_first (sorted)->data != &vertices[0]);
	CATCH (list_next (list_first (sorted))->data != &vertices[1]);
	CATCH (list_next (list_next (list_first (sorted)))->data != &vertices[2]);
	list_destroy (sorted);
	CATCH (topological_count_vertices (topological) != 3);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_9 (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 0 }, { 1 }, { 2 } };
	List *sorted;
	size_t i;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertices[0]));
	CATCH (!topological_add_vertex (topological, &vertices[1]));
	CATCH (!topological_add_vertex (topological, &vertices[2]));
	CATCH (!topological_set_edge (topological, &vertices[0], &vertices[1]));
	CATCH (!topological_set_edge (topological, &vertices[1], &vertices[2]));
	for (i = 0; i < 10; i++) {
		CATCH (!(sorted = topological_sort (topological)));
		CATCH (list_count (sorted) != 3);
		CATCH (list_first (sorted)->data != &vertices[0]);
		CATCH (list_next (list_first (sorted))->data != &vertices[1]);
		CATCH (list_next (list_next (list_first (sorted)))->data != &vertices[2]);
		list_destroy (sorted);
	}
	CATCH (topological_count_vertices (topological) != 3);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_10 (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 0 }, { 1 }, { 2 }, { 3 }, { 4 } };
	List *sorted;
	size_t i;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertices[0]));
	CATCH (!topological_add_vertex (topological, &vertices[1]));
	CATCH (!topological_add_vertex (topological, &vertices[2]));
	CATCH (!topological_add_vertex (topological, &vertices[3]));
	CATCH (!topological_add_vertex (topological, &vertices[4]));
	for (i = 0; i < 4; i++) {
		CATCH (!topological_set_edge (topological, &vertices[i], &vertices[i + 1]));
		CATCH (!(sorted = topological_sort (topological)));
		CATCH (list_last (sorted)->data != &vertices[i + 1]);
		list_destroy (sorted);
	}
	CATCH (topological_count_vertices (topological) != 5);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_11 (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 0 }, { 1 }, { 2 }, { 3 }, { 4 } };
	List *sorted;
	size_t i;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertices[0]));
	CATCH (!topological_add_vertex (topological, &vertices[1]));
	CATCH (!topological_add_vertex (topological, &vertices[2]));
	CATCH (!topological_add_vertex (topological, &vertices[3]));
	CATCH (!topological_add_vertex (topological, &vertices[4]));
	for (i = 0; i < 4; i++) {
		CATCH (!topological_set_edge (topological, &vertices[i], &vertices[i + 1]));
		CATCH (!(sorted = topological_sort (topological)));
		CATCH (list_last (sorted)->data != &vertices[i + 1]);
		list_destroy (sorted);
	}
	CATCH (!topological_set_edge (topological, &vertices[4], &vertices[0]));
	CATCH (topological_sort (topological));
	CATCH (topological_count_vertices (topological) != 5);
	topological_destroy (topological);
	PASS ();
}
 
bool test_topological_sort_12 (Test *test)
{
	Topological *topological;
	List *sorted;
	ListNode *vertex;
	Object vertices[1000];
	size_t i;

	TITLE ();
	CATCH (!shuffled_set ());
	CATCH (!(topological = topological_create ()));
	for (i = 0; i < 1000; i++) {
		vertices[i].id = i;
		CATCH (!topological_add_vertex (topological, &vertices[i]));
	}
	for (i = 0; i < 1000; i++) {
		if (shuffled[i] == 999) {
			continue;
		}
		CATCH (!topological_set_edge (topological, &vertices[shuffled[i]], &vertices[shuffled[i] + 1]));
	}
	CATCH (!(sorted = topological_sort (topological)));
	CATCH (list_count (sorted) != 1000);
	CATCH (topological_count_vertices (topological) != 1000);
	i = 0;
	for (vertex = list_first (sorted); vertex; vertex = list_next (vertex)) {
		CATCH (vertex->data != &vertices[i]);
		i++;
	}
	list_destroy (sorted);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_13 (Test *test)
{
	Topological *topological;
	Object vertices[1000];
	size_t i;

	TITLE ();
	CATCH (!shuffled_set ());
	CATCH (!(topological = topological_create ()));
	for (i = 0; i < 1000; i++) {
		vertices[i].id = i;
		CATCH (!topological_add_vertex (topological, &vertices[i]));
	}
	for (i = 0; i < 1000; i++) {
		if (shuffled[i] == 999) {
			continue;
		}
		CATCH (!topological_set_edge (topological, &vertices[shuffled[i]], &vertices[shuffled[i] + 1]));
	}
	CATCH (!topological_set_edge (topological, &vertices[777], &vertices[333])); 
	CATCH (topological_sort (topological));
	CATCH (topological_count_vertices (topological) != 1000);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_sort_14 (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 0 }, { 1 }, { 2 }, { 3 }, { 4 } };
	List *sorted;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertices[0]));
	CATCH (!topological_add_vertex (topological, &vertices[1]));
	CATCH (!topological_add_vertex (topological, &vertices[2]));
	CATCH (!topological_add_vertex (topological, &vertices[3]));
	CATCH (!topological_add_vertex (topological, &vertices[4]));
	CATCH (!topological_set_edge (topological, &vertices[0], &vertices[1]));
	CATCH (!topological_set_edge (topological, &vertices[0], &vertices[2]));
	CATCH (!topological_set_edge (topological, &vertices[0], &vertices[3]));
	CATCH (!topological_set_edge (topological, &vertices[1], &vertices[2]));
	CATCH (!topological_set_edge (topological, &vertices[2], &vertices[3]));
	CATCH (!topological_set_edge (topological, &vertices[1], &vertices[4]));
	CATCH (!topological_set_edge (topological, &vertices[2], &vertices[4]));
	CATCH (!topological_set_edge (topological, &vertices[3], &vertices[4]));
	CATCH (!(sorted = topological_sort (topological)));
	CATCH (list_count (sorted) != 5);
	CATCH (list_first (sorted)->data != &vertices[0]);
	CATCH (list_last (sorted)->data != &vertices[4]);
	CATCH (topological_count_vertices (topological) != 5);
	list_destroy (sorted);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_get_edges_invalid_argument_1 (Test *test)
{
	TITLE ();
	CATCH (topological_get_edges (NULL, NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 1);
	PASS ();
}

bool test_topological_get_edges_invalid_argument_2 (Test *test)
{
	Topological *topological;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (topological_get_edges (topological, NULL));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	CATCH (error_at (0).code != 2);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_get_edges_invalid_operation (Test *test)
{
	Topological *topological;
	Object u = { 0 };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (topological_get_edges (topological, &u));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorInvalidOperation);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_get_edges_function_call_1 (Test *test)
{
	Topological *topological;
	Object u = { 0 };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &u));
	memory_commit_limit (0);
	CATCH (topological_get_edges (topological, &u));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 1);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_get_edges_function_call_2 (Test *test)
{
	Topological *topological;
	Object u = { 0 };
	Object v = { 1 };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &u));
	CATCH (!topological_add_vertex (topological, &v));
	CATCH (!topological_set_edge (topological, &u, &v));
	memory_commit_limit (memory_commit_size () + sizeof (size_t) + sizeof (List));
	CATCH (topological_get_edges (topological, &u));
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorFunctionCall);
	CATCH (error_at (0).code != 2);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_get_edges (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 1 }, { 2 }, { 3 } };
	List *edges;

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertices[0]));
	CATCH (!topological_add_vertex (topological, &vertices[1]));
	CATCH (!topological_add_vertex (topological, &vertices[2]));
	CATCH (!topological_set_edge (topological, &vertices[0], &vertices[1]));
	CATCH (!(edges = topological_get_edges (topological, &vertices[0])));
	CATCH (list_count (edges) != 1);
	CATCH (list_first (edges)->data != &vertices[1]);
	list_destroy (edges);
	CATCH (!(edges = topological_get_edges (topological, &vertices[1])));
	CATCH (list_count (edges) != 0);
	list_destroy (edges);
	CATCH (!(edges = topological_get_edges (topological, &vertices[2])));
	CATCH (list_count (edges) != 0);
	list_destroy (edges);
	CATCH (!topological_set_edge (topological, &vertices[1], &vertices[0]));
	CATCH (!(edges = topological_get_edges (topological, &vertices[1])));
	CATCH (list_count (edges) != 1);
	CATCH (list_first (edges)->data != &vertices[0]);
	list_destroy (edges);
	CATCH (topological_count_vertices (topological) != 3);
	topological_destroy (topological);
	PASS ();
}

bool test_topological_count_vertices_invalid_argument (Test *test)
{
	TITLE ();
	CATCH (topological_count_vertices (NULL));
	CATCH (error_count () != 1);
	CATCH (error_at (0).error != ErrorInvalidArgument);
	PASS ();
}

bool test_topological_count_vertices (Test *test)
{
	Topological *topological;
	Object vertices[] = { { 1 }, { 2 }, { 3 } };

	TITLE ();
	CATCH (!(topological = topological_create ()));
	CATCH (!topological_add_vertex (topological, &vertices[0]));
	CATCH (!topological_add_vertex (topological, &vertices[1]));
	CATCH (!topological_add_vertex (topological, &vertices[2]));
	CATCH (topological_count_vertices (topological) != 3);
	topological_destroy (topological);
	PASS ();
}

static bool shuffled_set (void)
{
	TainioTree *random;
	unsigned long long value;
	size_t i;

	if (!random_open ()) {
		return false;
	}
	if (!(random = tainio_tree_create (0, 999))) {
		random_close ();
		return false;
	}
	for (i = 0; i < 1000; i++) {
		if (!tainio_tree_next (random, &value)) {
			tainio_tree_destroy (random);
			random_close ();
			return false;
		}
		shuffled[i] = (size_t)value;
	}
	tainio_tree_destroy (random);
	random_close ();
	return true;
}
