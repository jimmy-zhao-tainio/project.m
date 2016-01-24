#ifndef worker_internal_close_h
#define worker_internal_close_h

#include <lib.net.poll/poll.h>

void worker_internal_close (NetPoll *poll, NetPollConnection *connection);

#endif
