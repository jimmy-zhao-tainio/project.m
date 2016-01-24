#ifndef worker_write_h
#define worker_write_h

#include <lib.net.poll/poll.h>

void worker_write (NetPoll *poll, 
                   NetPollConnection *connection);

#endif
