#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/string.h>
#include <string.h>
#include <stdio.h>

#include "frame.h"

bool net_websocket_frame_create (NetWebsocketFrame *frame)
{
        if (!(frame->buffer = memory_create (1024))) {
                error (FunctionCall);
                return false;
        }
        frame->size = 1024;
        frame->length = 0;
        return true;
}

bool net_websocket_frame_append (NetWebsocketFrame *frame, char *buffer, size_t length)
{
        void *grown;

        while (frame->length + length > frame->size) {
                if (frame->size >= 1024 * 512) {
                        error (InvalidOperation);
                        return false;
                }
                if (!(grown = memory_grow (frame->buffer, frame->size * 2))) {
                        error (FunctionCall);
                        return false;
                }
                frame->buffer = grown;
                frame->size = frame->size * 2;
        }
        (void)memcpy (frame->buffer + frame->length, buffer, length);
        frame->length = frame->length + length;
        return true;
}

bool net_websocket_frame_available (NetWebsocketFrame *frame)
{
        if (frame->frame_size == 0) {
                // Check if we can read the frame_size.
                return false;
        }
        if (frame->frame_size > frame->length) {
                return false;
        }
        return true;
}

void net_websocket_frame_destroy (NetWebsocketFrame *frame)
{
        if (frame->buffer) {
                memory_destroy (frame->buffer);
                frame->buffer = NULL;
        }
}
