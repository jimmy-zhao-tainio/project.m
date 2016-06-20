#ifndef lib_net_websocket_frame_h
#define lib_net_websocket_frame_h

typedef struct _NetWebsocketFrame NetWebsocketFrame;

struct _NetWebsocketFrame {
        char *buffer;
        size_t size;
        size_t length;
        size_t frame_size;
};

bool net_websocket_frame_create (NetWebsocketFrame *frame);
bool net_websocket_frame_append (NetWebsocketFrame *frame, char *buffer, size_t length);
bool net_websocket_frame_available (NetWebsocketFrame *frame);
void net_websocket_frame_destroy (NetWebsocketFrame *frame);

#endif
