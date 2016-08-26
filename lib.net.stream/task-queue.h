#ifndef net_stream_task_queue_h
#define net_stream_task_queue_h

#include <lib.net.stream/structures.h>

NetStreamTaskQueue *net_stream_task_queue_create (void);
void net_stream_task_queue_destroy (NetStreamTaskQueue *task_queue);
bool net_stream_task_queue_push (NetStreamTaskQueue *task_queue,
                                 NetStreamTask task);
NetStreamTask net_stream_task_queue_pop (NetStreamTaskQueue *task_queue);

#endif
