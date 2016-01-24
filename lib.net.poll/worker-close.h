#ifndef worker_close_h
#define worker_close_h

#include <lib.net.poll/poll.h>

void worker_close (NetPoll *poll, NetPollConnection *connection);

#endif
