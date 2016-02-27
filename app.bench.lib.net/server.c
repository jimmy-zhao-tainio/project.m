#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.stream/stream.h>
#include <lib.net.server/server.h>
#include <unistd.h>
#include <stdio.h>

#include "server.h"

static void server_on_connect (NetServer *server, int socket);
static void server_on_error   (NetServer *server);

static void server_stream_on_add (NetStream *stream, NetStreamConnection *connection);
static void server_stream_on_close (NetStream *stream, NetStreamConnection *connection);
static void server_stream_on_read (NetStream *stream, NetStreamConnection *connection, 
                                   unsigned char *buffer, size_t length);

static NetServer *server;
static NetStream *stream;

static void worker (Thread *thread);
static ThreadSignal worker_start = THREAD_SIGNAL_INITIALIZER;
static bool worker_is_started = false;

static ThreadLock lock = THREAD_LOCK_INITIALIZER;

static size_t read_bytes = 0;
static size_t read_packages = 0;

static int msleep(unsigned long milisec);
static size_t seconds;

static size_t connections;
static size_t package_size;

void server_start (size_t _connections, size_t _package_size, size_t _seconds)
{
        seconds = _seconds;
        connections = _connections;
        package_size = _package_size;

        server = net_server_create ("127.0.0.1", 8888,
                                    &server_on_connect,
                                    &server_on_error,
                                    NULL);
        stream = net_stream_create (&server_stream_on_add, 
                                    &server_stream_on_close, 
                                    &server_stream_on_read);
        (void)thread_create (&worker, NULL);
}

static void server_on_connect (NetServer *server, int socket)
{
        (void)server;
        if (!net_stream_add (stream, socket)) {
                printf ("server_on_connect\n");
                exit (-1);
        }
}

static void server_on_error (NetServer *server)
{
        (void)server;
        printf ("server_on_error\n");
        exit (-1);
}

static void server_stream_on_add (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
}

static void server_stream_on_close (NetStream *stream, NetStreamConnection *connection)
{
        (void)stream;
        (void)connection;
        printf ("server_stream_on_close\n");
        exit (-1);
}

static void server_stream_on_read (NetStream *stream, NetStreamConnection *connection, 
                                   unsigned char *buffer, size_t length)
{
        (void)stream;
        (void)connection;
        (void)buffer;

        if (!worker_is_started) {
                thread_signal (&worker_start);
        }
        thread_lock (&lock);
        read_bytes += length;
        read_packages += 1;
        thread_unlock (&lock);
}

static void worker (Thread *thread)
{
        size_t i = 0;
        size_t *total_bytes;
        size_t *total_packages;

        size_t total_index = 0;
        size_t total_size = 1000 * seconds;

        (void)thread;
        total_bytes = memory_create (sizeof (size_t) * total_size);
        total_packages = memory_create (sizeof (size_t) * total_size);
        thread_signal_wait (&worker_start);
        for (i = 0; i < total_size; i++) {
                msleep (1);
                thread_lock (&lock);
                total_bytes[total_index] = read_bytes;
                total_packages[total_index] = read_packages;
                read_bytes = 0;
                read_packages = 0;
                thread_unlock (&lock);
                total_index++;
        }
        for (i = 0; i < total_size; i++) {
                printf ("%zu\t%zu\t%zu\t%zu\t%zu\n", 
                        i,
                        connections, 
                        package_size, 
                        total_packages[i], 
                        total_bytes[i]);
        }
        fflush (stdout);
        exit (0);
}

static int msleep(unsigned long milisec)
{
        struct timespec req = {0};
        time_t sec = (int)0;

        req.tv_sec = sec;
        req.tv_nsec = (long int)milisec * 1000000L;

        while (nanosleep (&req, &req) == -1)
                continue;
        return 1;
}
