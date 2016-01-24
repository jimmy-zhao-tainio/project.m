#ifndef worker_read_h
#define worker_read_h

#include <lib.net.poll/poll.h>

void worker_read (NetPoll *poll, NetPollConnection *connection);

#endif
