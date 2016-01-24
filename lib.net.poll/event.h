#ifndef net_poll_event_h
#define net_poll_event_h

#include <lib.core/defines.h>
#include <lib.core/threads-lock.h>
#include <lib.core/threads-signal.h>
#include <lib.net.poll/connection.h>

typedef struct _NetPollEvent NetPollEvent;

struct _NetPollEvent
{
        NetPollConnection *connection;
        enum {
                NET_POLL_EMPTY = 1,
                NET_POLL_INTERNAL_MONITOR = 2,
                NET_POLL_INTERNAL_CLOSE = 4,
                NET_POLL_INTERNAL_WRITE = 8,
                NET_POLL_READ = 16,
                NET_POLL_WRITE = 32,
                NET_POLL_CLOSE = 64,
                NET_POLL_EXIT = 128
        } type;
};

#endif
