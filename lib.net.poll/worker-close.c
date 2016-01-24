#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <sys/epoll.h>
#include <unistd.h>

void worker_close (NetPoll *poll, NetPollConnection *connection)
{
        if (!poll->on_close) {
                return;
        }
        poll->on_close (poll, connection, true);
}
