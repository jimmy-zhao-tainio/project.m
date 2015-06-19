#ifndef list_h
#define list_h

#include <lib.core/defines.h>

typedef struct _List     List;
typedef struct _ListItem ListItem;
typedef struct _ListNode ListNode;

struct _List
{
	ListItem *first;
	ListItem *last;

	size_t count;
};

struct _ListItem
{
        ListItem *previous;
        ListItem *next;
};

struct _ListNode
{
        ListItem item;
	void *data;
};

List *list_create  (void);
void  list_destroy (List *list);

ListNode *list_insert (List *list, void *data);
ListItem *list_item_insert (List *list, ListItem *item);
ListNode *list_append (List *list, void *data);
ListItem *list_item_append (List *list, ListItem *item);

bool list_position (List *list, ListNode *node, ListNode *next);
void list_remove   (List *list, ListNode *node);
bool list_item_position (List *list, ListItem *item, ListItem *next);
void list_item_remove   (List *list, ListItem *item);

ListNode *list_first (const List *list);
ListNode *list_last  (const List *list);
ListNode *list_find  (const List *list, const void *data);
ListNode *list_next  (const ListNode *node);
ListNode *list_previous (const ListNode *node);

ListItem *list_item_first (const List *list);
ListItem *list_item_last  (const List *list);
ListItem *list_item_next  (const ListItem *item);
ListItem *list_item_previous (const ListItem *item);

size_t list_count (const List *list);

#endif
