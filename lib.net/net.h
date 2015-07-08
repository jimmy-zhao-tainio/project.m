#ifndef lib_net_h
#define lib_net_h

#include <lib.core/defines.h>
#include <lib.core/object.h>
#include <lib.net/structures.h>

NetServer *net_server_create (NetAddress address,
                              NetOnConnect on_connect, 
                              NetOnClose on_close, 
                              NetOnRead on_read, 
                              NetOnError on_error);
NetClient *net_client_create (NetOnConnect on_connect, 
                              NetOnClose on_close, 
                              NetOnRead on_read, 
                              NetOnError on_error);
void net_destroy (Net *net);
void net_close   (Net *net, NetConnection connection);
void net_write   (Net *net, NetConnection connection, NetPackage package);
bool net_connect (Net *net, NetAddress address);

#endif
