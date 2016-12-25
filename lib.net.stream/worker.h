#ifndef net_stream_worker_h
#define net_stream_worker_h

#include <lib.net.stream/structures.h>

NetStreamWorker *net_stream_worker_create  (NetPoll *poll);
void             net_stream_worker_destroy (NetStreamWorker *worker);
bool net_stream_worker_add (NetStreamWorker *worker, 
                            NetStreamConnection *connection, 
                            NetStreamTask task);

#endif
