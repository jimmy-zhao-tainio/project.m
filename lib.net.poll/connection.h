#ifndef net_poll_connection_h
#define net_poll_connection_h

#include <lib.core/threads-lock.h>

typedef struct _NetPollConnection NetPollConnection;

struct _NetPollConnection
{
        int socket;
        void *pointer;
        struct {
                unsigned char *buffer;
                size_t length;
                size_t position;
                bool monitor;
        } write;
        bool closed;
};

#endif
