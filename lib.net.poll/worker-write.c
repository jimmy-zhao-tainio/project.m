#include <lib.net.poll/worker-write.h>
#include <lib.net.poll/worker-internal-close.h>
#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

static void monitor_write (NetPoll *poll, NetPollConnection *connection, bool monitor);

static void on_write (NetPoll *poll, NetPollConnection *connection, bool success);
static void free_on_write (NetPollConnection *connection);
static void close_on_write (NetPoll *poll, NetPollConnection *connection);

void worker_write (NetPoll *poll, 
                   NetPollConnection *connection)
{
        ssize_t count;

        if (connection->closed) {
                on_write (poll, connection, false);
                free_on_write (connection);
                return;
        }
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
                        on_write (poll, connection, false);
                        worker_internal_close (poll, connection);
                        free_on_write (connection);
                        return;
                }
                if (count == 0) {
                        /* Not specified */
                        on_write (poll, connection, false);
                        worker_internal_close (poll, connection);
                        free_on_write (connection);
                        return;
                }
                connection->write.position += (size_t)count;
        }
        on_write (poll, connection, true);
        free_on_write (connection);
        close_on_write (poll, connection);
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

static void on_write (NetPoll *poll, NetPollConnection *connection, bool success)
{
        if (poll->on_write) {
                poll->on_write (poll, 
                                connection, 
                                connection->write.buffer, 
                                connection->write.length,
                                success);
        }
}

static void free_on_write (NetPollConnection *connection)
{
        if (!(connection->write.flags & FREE_BUFFER)) {
                return;
        }
        if (!connection->write.buffer) {
                return;
        }
        memory_destroy (connection->write.buffer);
        connection->write.buffer = NULL;
        if (connection->write.flags & CLOSE_ON_WRITE) {
                connection->write.flags = CLOSE_ON_WRITE;
        }
        else {
                connection->write.flags = 0;
        }
}

static void close_on_write (NetPoll *poll, NetPollConnection *connection)
{
        if (!(connection->write.flags & CLOSE_ON_WRITE)) {
                return;
        }
        worker_internal_close (poll, connection);
        if (connection->write.flags & FREE_BUFFER) {
                connection->write.flags = FREE_BUFFER;
        }
        else {
                connection->write.flags = 0;
        }
}
