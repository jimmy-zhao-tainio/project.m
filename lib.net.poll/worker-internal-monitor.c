#include <lib.net.poll/poll.h>
#include <lib.core/error.h>
#include <sys/epoll.h>

void worker_internal_monitor (NetPoll *poll, NetPollConnection *connection)
{
        struct epoll_event event = { 0 };

        if (connection->closed) {
                return;
        }
        event.data.ptr = connection;
        event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLET;
        if (epoll_ctl (poll->events->file,
                       EPOLL_CTL_ADD,
                       connection->socket,
                       &event) == -1) {
                error (SystemCall);
                if (!poll->on_monitor) {
                        return;
                }
                poll->on_monitor (poll, connection, false);
        }
        else {
                if (!poll->on_monitor) {
                        return;
                }
                poll->on_monitor (poll, connection, true);
        }
}
