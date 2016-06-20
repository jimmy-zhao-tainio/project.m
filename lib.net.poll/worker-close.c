#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>

void worker_close (NetPoll *poll, NetPollConnection *connection)
{
        close (connection->socket);
        if (connection->closed) {
                return;
        }
        connection->closed = true;
        if (!poll->on_close) {
                return;
        }
        poll->on_close (poll, connection, true);
}
