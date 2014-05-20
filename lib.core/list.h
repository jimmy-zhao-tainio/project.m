#ifndef list_h
#define list_h

#include <lib.core/defines.h>

typedef struct _List     List;
typedef struct _ListNode ListNode;

struct _List
{
	ListNode *first;
	ListNode *last;

	size_t count;
};

struct _ListNode
{
	ListNode *previous;
	ListNode *next;

	void *data;
};

List *list_create  (void);
void  list_destroy (List *list);

ListNode *list_insert (List *list, void *data);
ListNode *list_append (List *list, void *data);

bool list_position (List *list, ListNode *node, ListNode *next);
void list_remove   (List *list, ListNode *node);

ListNode *list_first (const List *list);
ListNode *list_last  (const List *list);
ListNode *list_find  (const List *list, const void *data);

size_t list_count (const List *list);

#endif
