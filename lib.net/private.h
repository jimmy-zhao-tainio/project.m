#ifndef lib_net_private_h
#define lib_net_private_h

#include <lib.core/defines.h>
#include <lib.core/object.h>
#include <lib.net/private-structures.h>

bool net_private_create  (NetPrivate *net);
void net_private_destroy (NetPrivate *net);

#endif
