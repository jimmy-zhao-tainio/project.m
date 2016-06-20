#include <lib.net.poll/poll.h>
#include <lib.net.poll/worker.h>
#include <lib.net.poll/worker-internal-monitor.h>
#include <lib.net.poll/worker-internal-close.h>
#include <lib.net.poll/worker-close.h>
#include <lib.net.poll/worker-read.h>
#include <lib.net.poll/worker-write.h>

void net_poll_worker (Thread *thread)
{
        NetPoll *poll = thread->argument;
        NetPollEvent event;

        while (true) {
                event = net_poll_events_next (poll->events);
                if (event.type == NET_POLL_EMPTY) {
                        continue;
                }
                if (event.type == NET_POLL_INTERNAL_MONITOR) {
                        worker_internal_monitor (poll, event.connection);
                        continue;
                }
                if (event.type == NET_POLL_INTERNAL_CLOSE) {
                        worker_internal_close (poll, event.connection);
                        net_poll_events_clear (poll->events, event.connection);
                        continue;
                }
                if (event.type == NET_POLL_INTERNAL_WRITE) {
                        worker_write (poll, event.connection);
                        continue;
                }
                if (event.type == NET_POLL_CLOSE) {
                        worker_close (poll, event.connection);
                        net_poll_events_clear (poll->events, event.connection);
                        continue;
                }
                if (event.type == (NET_POLL_READ | NET_POLL_WRITE)) {
                        worker_read (poll, event.connection);
                        worker_write (poll, event.connection);
                        continue;
                }
                if (event.type == NET_POLL_READ) {
                        worker_read (poll, event.connection);
                        continue;
                }
                if (event.type == NET_POLL_WRITE) {
                        worker_write (poll, event.connection);
                        continue;
                }
                if (event.type == NET_POLL_EXIT) {
                        return;
                }
        }
}
