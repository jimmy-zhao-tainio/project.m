#include <lib.net.poll/poll.h>
#include <lib.net.poll/worker-internal-close.h>
#include <lib.core/error.h>
#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>

void worker_read (NetPoll *poll, NetPollConnection *connection)
{
        unsigned char buffer[512];
        ssize_t count;

        if (connection->closed) {
                return;
        }
        while (true) {
                count = read (connection->socket, buffer, sizeof (buffer));
                if (count == 0) {
                        worker_internal_close (poll, connection);
                        return;
                }
                if (count == -1) {
                        if (errno == EINTR) {
                                continue;
                        }
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                return;
                        }
                        else {
                                worker_internal_close (poll, connection);
                                return;
                        }
                }
                if (!poll->on_read) {
                        continue;
                }
                poll->on_read (poll, connection, buffer, (size_t)count);
        }
}
