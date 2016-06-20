#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.websocket/websocket.h>
#include <lib.net.websocket/frame.h>
#include <lib.core/string.h>
#include <unistd.h>
#include <stdio.h>

#include "test-frame.h"

bool test_frame_create_function_call (Test *test)
{
        NetWebsocketFrame frame = { 0 };

        TITLE ();
        memory_commit_limit (0);
        CATCH (net_websocket_frame_create (&frame));
        CATCH (error_at (0).error != ErrorFunctionCall);
        PASS ();
}

bool test_frame_create (Test *test)
{
        NetWebsocketFrame frame = { 0 };

        TITLE ();
        CATCH (!net_websocket_frame_create (&frame));
        net_websocket_frame_destroy (&frame);
        PASS ();
}

bool test_frame_append_invalid_operation (Test *test)
{
        NetWebsocketFrame frame = { 0 };
        char *buffer;

        TITLE ();
        CATCH (!(buffer = memory_create (1024 * 513)));
        CATCH (!net_websocket_frame_create (&frame));
        CATCH (net_websocket_frame_append (&frame, buffer, 1024 * 513));
        CATCH (error_at (0).error != ErrorInvalidOperation);
        net_websocket_frame_destroy (&frame);
        memory_destroy (buffer);
        PASS ();
}

bool test_frame_append_function_call (Test *test)
{
        NetWebsocketFrame frame = { 0 };
        char *buffer;

        TITLE ();
        CATCH (!net_websocket_frame_create (&frame));
        CATCH (!(buffer = memory_create (1024 * 512)));
        memory_commit_limit (0);
        CATCH (net_websocket_frame_append (&frame, buffer, 1024 * 512));
        CATCH (error_at (0).error != ErrorFunctionCall);
        net_websocket_frame_destroy (&frame);
        memory_destroy (buffer);
        PASS ();
}

bool test_frame_append (Test *test)
{
        NetWebsocketFrame frame = { 0 };

        TITLE ();
        CATCH (!net_websocket_frame_create (&frame));
        CATCH (!net_websocket_frame_append (&frame, "abc", 3));
        CATCH (!net_websocket_frame_append (&frame, "def", 3));
        net_websocket_frame_destroy (&frame);
        PASS ();
}
