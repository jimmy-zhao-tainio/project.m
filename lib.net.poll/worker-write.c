#include <lib.net.poll/worker-write.h>
#include <lib.net.poll/worker-internal-close.h>
#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

// 2648086

static void monitor_write (NetPoll *poll, NetPollConnection *connection, bool monitor);

void worker_write (NetPoll *poll, 
                   NetPollConnection *connection)
{
        ssize_t count;

        if (connection->write.monitor == true) {
                monitor_write (poll, connection, false);
        }
        while (connection->write.position < connection->write.length) {
                count = write (connection->socket,
                               connection->write.buffer + connection->write.position,
                               connection->write.length - connection->write.position);

                if (count == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                monitor_write (poll, connection, true);
                                return;
                        }
                        if (errno == EINTR) {
                                continue;

                        }
                        worker_internal_close (poll, connection);
                        return;
                }
                if (count == 0) {
                        /* Not specified */
                        worker_internal_close (poll, connection);
                        return;
                }
                connection->write.position += (size_t)count;
        }
        if (!thread_unlock (&connection->write.lock)) {
                error (FunctionCall);
        }
}

static void monitor_write (NetPoll *poll, NetPollConnection *connection, bool monitor)
{
        struct epoll_event event = { 0 };

        event.data.ptr = connection;
        if (monitor) {
                event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLHUP | EPOLLET;
        }
        else {
                event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLET;
        }
        if (epoll_ctl (poll->events->file,
                       EPOLL_CTL_MOD,
                       connection->socket,
                       &event) == -1) {
                error (SystemCall);
                worker_internal_close (poll, connection);
        }
        connection->write.monitor = monitor;
}
