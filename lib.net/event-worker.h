#ifndef lib_net_event_worker_h
#define lib_net_event_worker_h

#include <lib.core/defines.h>
#include <lib.core/object.h>
#include <lib.net/private-structures.h>

void event_worker        (Thread *thread);

void event_server_error     (NetPrivate *net);
void event_server_accept    (NetPrivate *net);
bool event_connection_open  (NetPrivate *net, NetPrivateConnection *connection);
void event_connection_read  (NetPrivate *net, NetPrivateConnection *connection);
void event_connection_write (NetPrivate *net, NetPrivateConnection *connection);
void event_connection_close (NetPrivate *net, NetPrivateConnection *connection);

bool connection_add    (NetPrivate *net, int socket);
void connection_remove (NetPrivate *net, NetPrivateConnection *connection);

#endif
