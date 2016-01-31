#ifndef net_poll_h
#define net_poll_h

#include <lib.net.poll/events.h>
#include <lib.net.poll/connection.h>
#include <lib.core/threads.h>

typedef struct _NetPoll NetPoll;

typedef void (*NetPollOnMonitor) (NetPoll *poll, 
                                  NetPollConnection *connection, 
                                  bool success);
typedef void (*NetPollOnClose)   (NetPoll *poll, 
                                  NetPollConnection *connection, 
                                  bool success);
typedef void (*NetPollOnRead)    (NetPoll *poll,
                                  NetPollConnection *connection, 
                                  unsigned char *buffer,
                                  size_t length);
typedef void (*NetPollOnWrite)   (NetPoll *poll,
                                  NetPollConnection *connection, 
                                  unsigned char *buffer,
                                  size_t length);

struct _NetPoll
{
        NetPollEvents *events;
        Thread *worker;
        NetPollOnMonitor on_monitor;
        NetPollOnClose   on_close;
        NetPollOnRead    on_read;
        NetPollOnWrite   on_write;
        void *pointer;
};

NetPoll *net_poll_create  (NetPollOnMonitor on_monitor,
                           NetPollOnClose on_close,
                           NetPollOnRead on_read,
                           NetPollOnWrite on_write);
void     net_poll_destroy (NetPoll *poll);
bool     net_poll_monitor (NetPoll *poll, NetPollConnection *connection);
bool     net_poll_close   (NetPoll *poll, NetPollConnection *connection);
bool     net_poll_write   (NetPoll *poll, NetPollConnection *connection,
                           unsigned char *buffer, size_t length);

#endif
