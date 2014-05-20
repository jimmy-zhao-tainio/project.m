#ifndef object_h
#define object_h

#include <lib.core/defines.h>

typedef struct _Object Object;

struct _Object
{
	unsigned long long id;
};

unsigned long long object_id (void);

#endif
