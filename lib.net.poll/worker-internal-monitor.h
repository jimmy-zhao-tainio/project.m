#ifndef worker_internal_monitor_h
#define worker_internal_monitor_h

#include <lib.net.poll/poll.h>

void worker_internal_monitor (NetPoll *poll, NetPollConnection *connection);

#endif
