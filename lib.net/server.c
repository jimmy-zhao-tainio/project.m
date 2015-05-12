#include <lib.net/server.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <lib.app/events.h>

typedef struct _Epoll Epoll;

struct _Epoll 
{
        struct epoll_event event;
        struct epoll_event *events;
        int events_length;
        int file;
        int event_stop;
};

#define EPOLL(x) ((Epoll *)x->private)

static bool start (Server *server);
static void stop (Server *server);
static void worker (Thread *thread);
static bool epoll_construct (Server *server);
static void epoll_destruct (Server *server);
static bool epoll_nonblocking (int socket);
static bool epoll_events_count (Server *server, int *count);
static bool epoll_handle_events (Server *server, int count);
static bool epoll_handle_event (Server *server, struct epoll_event event);
static bool epoll_handle_event_server (Server *server, struct epoll_event event);
static bool epoll_handle_event_client (Server *server, struct epoll_event event);
static bool epoll_stop (Server *server);

Server *server_create (uint16_t port, 
                       EventOpen on_open, 
                       EventClose on_close, 
                       EventRead on_read, 
                       EventStop on_stop)
{
        Server *server;

        if (port == 0) {
                error_code (InvalidArgument, 1);
                return NULL;
        }
        if (!on_open) {
                error_code (InvalidArgument, 2);
                return NULL;
        }
        if (!on_close) {
                error_code (InvalidArgument, 3);
                return NULL;
        }
        if (!on_read) {
                error_code (InvalidArgument, 4);
                return NULL;
        }
        if (!on_stop) {
                error_code (InvalidArgument, 5);
                return NULL;
        }
        if (!(server = memory_create (sizeof (Server)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        server->port = port;
        server->on_open = on_open;
        server->on_close = on_close;
        server->on_read = on_read;
        server->on_stop = on_stop;
        server->socket = -1;
        if (!start (server)) {
                server_destroy (server);
                error_code (FunctionCall, 2);
                return NULL;
        }
        return server;
}

void server_destroy (Server *server)
{
        if (!server) {
                error (InvalidArgument);
                return;
        }
        stop (server);
        epoll_destruct (server);
        memory_destroy (server);
}

static bool start (Server *server)
{
        struct sockaddr_in server_address;

        if ((server->socket = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
                error_code (SystemCall, 1);
                return false;
        }
        bzero (&server_address, sizeof (server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_port = htons (server->port);
        server_address.sin_addr.s_addr = htonl (INADDR_ANY);
        if (bind (server->socket, (struct sockaddr *)&server_address, 
                                   sizeof (server_address)) == -1) {
                close (server->socket);
                error_code (SystemCall, 2);
                return false;
        }
        if (!epoll_nonblocking (server->socket)) {
                close (server->socket);
                error_code (SystemCall, 3);
                return false;
        }
        if (listen (server->socket, SOMAXCONN) == -1) {
                close (server->socket);
                error_code (SystemCall, 4);
                return false;
        }
        if (!epoll_construct (server)) {
                close (server->socket);
                error_code (FunctionCall, 1);
                return false;
        }
        if (!(server->thread = thread_create (&worker, server))) {
                close (server->socket);
                epoll_destruct (server);
                error_code (FunctionCall, 2);
                return false;
        }
        return true;
}

static void stop (Server *server)
{
        if (server->thread) {
                thread_set_exit (server->thread);
                epoll_stop (server);
                thread_wait (server->thread);
                thread_destroy (server->thread);
        }
        close (server->socket);
}

static void worker (Thread *thread)
{
        Server *server = thread->argument;
        int count;

        while (epoll_events_count (server, &count)) {
                if (!epoll_handle_events (server, count)) {
                        server->on_stop (server);
                        error (FunctionCall);
                        return;
                }
        }
}

static bool epoll_events_count (Server *server, int *count)
{
        while (true) {
                *count = epoll_wait (EPOLL (server)->file,
                                     EPOLL (server)->events,
                                     EPOLL (server)->events_length,
                                     -1);
                if (*count > 0) {
                        return true;
                }
                else if (*count < 0) {
                        if (errno == EINTR) {
                                if (thread_get_exit (server->thread)) {
                                        return false;
                                }
                                error_code (SystemCall, errno);
                                continue;
                        }
                        error_code (SystemCall, errno);
                        return false;
                }
        }
}

static bool epoll_handle_events (Server *server, int count)
{
        int i;

        for (i = 0; i < count; i++) {
                if (!epoll_handle_event (server, EPOLL (server)->events[i])) {
                        error (FunctionCall);
                        return false;
                }
        }
        return true;
}

static bool epoll_handle_event (Server *server, struct epoll_event event)
{
        if (event.data.ptr == server) {
                return epoll_handle_event_server (server, event);
        }
        else if (event.data.ptr == &EPOLL (server)->event_stop) {
                return false;
        }
        else {
                return epoll_handle_event_client (server, event);
        }
}

static bool epoll_handle_event_server (Server *server, struct epoll_event event)
{
        int socket;

        if (event.events & EPOLLERR ||
            event.events & EPOLLHUP) {
                error (SystemCall);
                return false;
        }
        if (!(event.events & EPOLLIN)) {
                error (SystemCall);
                return false;
        }
        while (true) {
                if ((socket = accept (server->socket, NULL, NULL)) == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                return true;
                        }
                        // Check more errno's and decide what is critical.
                        error_code (SystemCall, errno);
                        continue;
                }
                if (!epoll_nonblocking (socket)) {
                        close (socket);
                        error (FunctionCall);
                        continue;
                }
                event.events = EPOLLIN | EPOLLET;
                if (epoll_ctl (EPOLL (server)->file,
                               EPOLL_CTL_ADD,
                               socket,
                               &event) == -1) {
                        close (socket);
                        continue;
                }
                event.data.ptr = server->on_open (server, socket);
        }
}

static bool epoll_handle_event_client (Server *server, struct epoll_event event)
{
        Client *client;
        char buffer[512];
        ssize_t count;

        client = (Client *)event.data.ptr;
        if (event.events & EPOLLERR ||
            event.events & EPOLLHUP) {
                server->on_close (server, client);
                return true;
        }
        if (!(event.events & EPOLLIN)) {
                server->on_close (server, client);
                return true;
        }
        while (true) {
                count = read (client->socket, buffer, sizeof (buffer));
                if (count == 0) {
                        close (client->socket);
                        server->on_close (server, client);
                        return true;
                }
                if (count == -1) {
                        if (errno == EAGAIN) {
                                return true;
                        }
                        close (client->socket);
                        server->on_close (server, client);
                        return true;
                }
                server->on_read (server, client, buffer, (size_t)count);
        }
        return true;
}

static bool epoll_construct (Server *server)
{
        int events_length = 64;
        size_t events_bytes = (size_t)((size_t)events_length * (size_t)sizeof (struct epoll_event));
        struct epoll_event event = { 0 };

        if (!(server->private = memory_create (sizeof (Epoll)))) {
                error_code (FunctionCall, 1);
                return false;
        }
        EPOLL (server)->event_stop = -1;
        EPOLL (server)->file = -1;
        EPOLL (server)->events = NULL;
        if ((EPOLL (server)->file = epoll_create1 (0)) == -1) {
                epoll_destruct (server);
                error_code (SystemCall, 1);
                return false;
        }
        EPOLL (server)->event.data.ptr = server;
        EPOLL (server)->event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl (EPOLL (server)->file,
                       EPOLL_CTL_ADD,
                       server->socket,
                       &EPOLL (server)->event) == -1) {
                epoll_destruct (server);
                error_code (SystemCall, 2);
                return false;
        }
        EPOLL (server)->events_length = events_length;
        if (!(EPOLL (server)->events = memory_create (events_bytes))) {
                epoll_destruct (server);
                error_code (SystemCall, 3);
                return false;
        }
        if ((EPOLL (server)->event_stop = eventfd (0, EFD_NONBLOCK)) == -1) {
                error_code (SystemCall, 4);
                epoll_destruct (server);
                return false;
        }
        event.data.ptr = &EPOLL (server)->event_stop;
        event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl (EPOLL (server)->file, EPOLL_CTL_ADD, EPOLL (server)->event_stop, &event) == -1) {
                error_code (SystemCall, 5);
                epoll_destruct (server);
                return false;
        }
        return true;
}

static void epoll_destruct (Server *server)
{
        if (!EPOLL (server)) {
                return;
        }
        if (EPOLL (server)->event_stop != -1) {
                close (EPOLL (server)->event_stop);
        }
        if (EPOLL (server)->file != -1) {
                close (EPOLL (server)->file);
        }
        if (EPOLL (server)->events) {
                memory_destroy (EPOLL (server)->events);
        }
        memory_destroy (EPOLL (server));
        server->private = NULL;
}

static bool epoll_nonblocking (int socket)
{
        int flags;

        if ((flags = fcntl (socket, F_GETFL, 0)) == -1) {
                error_code (SystemCall, 1);
                return false;
        }
        flags |= O_NONBLOCK;
        if (fcntl (socket, F_SETFL, flags) == -1) {
                error_code (SystemCall, 2);
                return false;
        }
        return true;
}

static bool epoll_stop (Server *server)
{
        if (eventfd_write (EPOLL (server)->event_stop, 1) == -1) {
                error_code (SystemCall, errno);
                return false;
        }
        return true;
}
