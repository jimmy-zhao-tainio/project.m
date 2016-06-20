#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>

void worker_internal_close (NetPoll *poll, NetPollConnection *connection)
{
        if (connection->closed) {
                return;
        }
        connection->closed = true;
        if (close (connection->socket) != 0) {
                error (SystemCall);
                if (!poll->on_close) {
                        return;
                }
                poll->on_close (poll, connection, false);
        }
        else {
                if (!poll->on_close) {
                        return;
                }
                poll->on_close (poll, connection, true);
        }
}
