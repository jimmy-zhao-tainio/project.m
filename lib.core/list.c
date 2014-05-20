#include <lib.core/list.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

static void list_insert_at (List *list, ListNode *node, ListNode *next);
static void list_remove_without_destroy (List *list, ListNode *node);

List *list_create (void)
{
	List *list;

	if (!(list = memory_create (sizeof (List)))) {
		error (FunctionCall);
		return NULL;
	}
	return list;
}

void list_destroy (List *list)
{
	ListNode *node;

	if (!list) {
		error (InvalidArgument);
		return;
	}
	for (node = list->first; node; node = list->first) {
		list->first = node->next;
		memory_destroy (node);
	}
	memory_destroy (list);
}

ListNode *list_insert (List *list, void *data)
{
	ListNode *node;

	if (!list) {
		error (InvalidArgument);
		return NULL;
	}
	if (!size_t_add (list->count, 1, NULL)) {
		error (Overflow);
		return NULL;
	}
	if (!(node = memory_create (sizeof (ListNode)))) {
		error (FunctionCall);
		return NULL;
	}
	node->data = data;
	if (!list->first) {
		list->first = node;
		list->last = node;
	}
	else if (list->first == list->last) {
		node->next = list->first;
		list->first->previous = node;
		list->first = node;
	}
	else {
		node->next = list->first;
		list->first->previous = node;
		list->first = node;
	}
	list->count++;
	return node;
}

ListNode *list_append (List *list, void *data)
{
	ListNode *node;

	if (!list) {
		error (InvalidArgument);
		return NULL;
	}
	if (!size_t_add (list->count, 1, NULL)) {
		error (Overflow);
		return NULL;
	}
	if (!(node = memory_create (sizeof (ListNode)))) {
		error (FunctionCall);
		return NULL;
	}
	node->data = data;
	if (!list->first) {
		list->first = node;
		list->last = node;
	}
	else if (list->first == list->last) {
		list->last = node;
		list->first->next = list->last;
		list->last->previous = list->first;
	}
	else {
		list->last->next = node;
		node->previous = list->last;
		list->last = node;
	}
	list->count++;
	return node;
}

bool list_position (List *list, ListNode *node, ListNode *next)
{
	if (!list ||
	    !node ||
	    !next ||
	    node == next) {
		error (InvalidArgument);
		return false;
	}
	if (node->next == next) {
		return true;
	}
	list_remove_without_destroy (list, next);
	list_insert_at (list, node, next);
	return true;
}

void list_remove (List *list, ListNode *node)
{
	if (!list ||
	    !node) {
		error (InvalidArgument);
		return;
	}
	list_remove_without_destroy (list, node);
	memory_destroy (node);
}

ListNode *list_first (const List *list)
{
	if (!list) {
		return NULL;
	}
	return list->first;
}

ListNode *list_last (const List *list)
{
	if (!list) {
		return NULL;
	}
	return list->last;
}

size_t list_count (const List *list)
{
	if (!list) {
		error (InvalidArgument);
		return 0;
	}
	return list->count;
}

ListNode *list_find (const List *list, const void *data)
{
	ListNode *node;

	if (!list) {
		error (InvalidArgument);
		return NULL;
	}
	for (node = list->first; node; node = node->next) {
		if (node->data == data) {
			return node;
		}
	}
	return NULL;
}

static void list_remove_without_destroy (List *list, ListNode *node)
{
	if (!list ||
	    !node) {
		return;
	}
	if (node == list->first) {
		list->first = list->first->next;
		if (!list->first) {
			list->last = NULL;
		}
		else {
			list->first->previous = NULL;
			if (!list->first->next) {
				list->last = list->first;
			}
		}
	}
	else if (node == list->last) {
		list->last = node->previous;
		list->last->next = NULL;
	}
	else {
		node->previous->next = node->next;
		node->next->previous = node->previous;
	}
	node->previous = NULL;
	node->next = NULL;
	if (list->count != 0) {
		list->count--;
	}
}

static void list_insert_at (List *list, ListNode *node, ListNode *next)
{
	if (node == list->first) {
		if (!list->first->next) {
			list->first->next = next;
			next->previous = list->first;
			list->last = next;
		}
		else {
			list->first->next->previous = next;
			next->next = list->first->next;
			next->previous = list->first;
			list->first->next = next;
		}
	}
	else if (node == list->last) {
		list->last->next = next;
		next->previous = list->last;
		list->last = next;
	}
	else {
		node->next->previous = next;
		next->next = node->next;
		node->next = next;
		next->previous = node;
	}
	list->count++;
}
