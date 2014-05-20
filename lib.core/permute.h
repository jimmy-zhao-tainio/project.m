#ifndef permute_h
#define permute_h

#include <lib.core/defines.h>

typedef struct _Permute Permute;

Permute      *permute_create  (size_t length);
void          permute_destroy (Permute *permute);
const size_t *permute_next    (Permute *permute);

#endif
