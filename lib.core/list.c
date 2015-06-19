#include <lib.core/list.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

static void list_item_insert_at (List *list, ListItem *item, ListItem *next);
static void list_item_remove_without_destroy (List *list, ListItem *item);

List *list_create (void)
{
	List *list;

	if (!(list = memory_create (sizeof (List)))) {
		error (FunctionCall);
		return NULL;
	}
        list->first = NULL;
        list->last = NULL;
        list->count = 0;
	return list;
}

void list_destroy (List *list)
{
	ListItem *item;

	if (!list) {
		error (InvalidArgument);
		return;
	}
	for (item = list->first; item; item = list->first) {
		list->first = item->next;
		memory_destroy (item);
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
	return (ListNode *)list_item_insert (list, (ListItem *)node);
}

ListItem *list_item_insert (List *list, ListItem *item)
{
        if (!list->first) {
		list->first = item;
		list->last = item;
	}
	else if (list->first == list->last) {
		item->next = list->first;
		list->first->previous = item;
		list->first = item;
	}
	else {
		item->next = list->first;
		list->first->previous = item;
		list->first = item;
	}
	list->count++;
	return item;
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
        return (ListNode *)list_item_append (list, (ListItem *)node);
}

ListItem *list_item_append (List *list, ListItem *item)
{
        if (!list->first) {
		list->first = item;
		list->last = item;
	}
	else if (list->first == list->last) {
		list->last = item;
		list->first->next = list->last;
		list->last->previous = list->first;
	}
	else {
		list->last->next = item;
		item->previous = list->last;
		list->last = item;
	}
	list->count++;
	return item;
}

bool list_position (List *list, ListNode *node, ListNode *next)
{
        return list_item_position (list, (ListItem *)node, (ListItem *)next);
}

bool list_item_position (List *list, ListItem *item, ListItem *next)
{
	if (!list ||
	    !item ||
	    !next ||
	    item == next) {
		error (InvalidArgument);
		return false;
	}
	if (item->next == next) {
		return true;
	}
	list_item_remove_without_destroy (list, next);
	list_item_insert_at (list, item, next);
	return true;
}

void list_remove (List *list, ListNode *node)
{
        if (!list ||
	    !node) {
		error (InvalidArgument);
		return;
	}
        list_item_remove (list, (ListItem *)node);
        memory_destroy (node);
}

void list_item_remove (List *list, ListItem *item)
{
	if (!list ||
	    !item) {
		error (InvalidArgument);
		return;
	}
	list_item_remove_without_destroy (list, item);
}

ListNode *list_first (const List *list)
{
        return (ListNode *)list_item_first (list);
}

ListNode *list_last (const List *list)
{
        return (ListNode *)list_item_last (list);
}

ListNode *list_find (const List *list, const void *data)
{
	ListNode *node;

	if (!list) {
		error (InvalidArgument);
		return NULL;
	}
	for (node = list_first (list); 
             node; 
             node = list_next (node)) {
		if (node->data == data) {
			return node;
		}
	}
	return NULL;
}

ListNode *list_next (const ListNode *node)
{
        return (ListNode *)list_item_next ((ListItem *)node);
}

ListNode *list_previous (const ListNode *node)
{
        return (ListNode *)list_item_previous ((ListItem *)node);
}

ListItem *list_item_first (const List *list)
{
	if (!list) {
		return NULL;
	}
	return list->first;
}

ListItem *list_item_last (const List *list)
{
	if (!list) {
		return NULL;
	}
	return list->last;
}

ListItem *list_item_next (const ListItem *item)
{
        if (!item) {
                error (InvalidArgument);
                return NULL;
        }
        return item->next;
}

ListItem *list_item_previous (const ListItem *item)
{
        if (!item) {
                error (InvalidArgument);
                return NULL;
        }
        return item->previous;
}

size_t list_count (const List *list)
{
        if (!list) {
                error (InvalidArgument);
                return 0;
        }
        return list->count;
}

static void list_item_remove_without_destroy (List *list, ListItem *item)
{
	if (!list ||
	    !item) {
		return;
	}
	if (item == list->first) {
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
	else if (item == list->last) {
		list->last = item->previous;
		list->last->next = NULL;
	}
	else {
		item->previous->next = item->next;
		item->next->previous = item->previous;
	}
	item->previous = NULL;
	item->next = NULL;
	if (list->count != 0) {
		list->count--;
	}
}

static void list_item_insert_at (List *list, ListItem *item, ListItem *next)
{
        if (item == list->first) {
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
	else if (item == list->last) {
		list->last->next = next;
		next->previous = list->last;
		list->last = next;
	}
	else {
		item->next->previous = next;
		next->next = item->next;
		item->next = next;
		next->previous = item;
	}
	list->count++;
}
