#include <lib.net.poll/poll.h>
#include <lib.net.poll/worker.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

NetPoll *net_poll_create (NetPollOnMonitor on_monitor,
                          NetPollOnClose on_close,
                          NetPollOnRead on_read,
                          NetPollOnWrite on_write)
{
        NetPoll *poll;

        if (!(poll = memory_create (sizeof (NetPoll)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(poll->events = net_poll_events_create ())) {
                memory_destroy (poll);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(poll->worker = thread_create (&net_poll_worker, poll))) {
                net_poll_events_destroy (poll->events);
                memory_destroy (poll);
                error_code (FunctionCall, 3);
                return NULL;
        }
        poll->on_monitor = on_monitor;
        poll->on_close = on_close;
        poll->on_read = on_read;
        poll->on_write = on_write;
        return poll;
}

void net_poll_destroy (NetPoll *poll)
{
        if (!poll) {
                error (InvalidArgument);
                return;
        }
        if (!net_poll_events_push_exit (poll->events)) {
                error_code (FunctionCall, 1);
                return;
        }
        if (!thread_wait (poll->worker)) {
                error_code (FunctionCall, 2);
                return;
        }
        thread_destroy (poll->worker);
        net_poll_events_destroy (poll->events);
        memory_destroy (poll);
}

bool net_poll_monitor (NetPoll *poll, NetPollConnection *connection)
{
        return net_poll_events_push_monitor (poll->events, connection);
}

bool net_poll_close (NetPoll *poll, NetPollConnection *connection)
{
        return net_poll_events_push_close (poll->events, connection);
}

bool net_poll_write (NetPoll *poll, NetPollConnection *connection,
                     unsigned char *buffer, size_t length)
{
        connection->write.buffer = buffer;
        connection->write.length = length;
        connection->write.position = 0;
        return net_poll_events_push_write (poll->events, connection);
}
