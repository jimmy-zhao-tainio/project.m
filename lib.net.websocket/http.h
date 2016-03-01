#ifndef lib_net_http_h
#define lib_net_http_h

typedef struct _NetHttpReader NetHttpReader;
typedef struct _NetHttpRequest NetHttpRequest;

struct _NetHttpRequest {
        size_t begin;
        size_t end;
};

struct _NetHttpReader {
        NetHttpRequest request;
        char *buffer;
        size_t size;
        size_t index;
        bool initialized;
};

bool net_http_reader_create  (NetHttpReader *reader);
bool net_http_reader_append  (NetHttpReader *reader, char *buffer, size_t length);
void net_http_reader_destroy (NetHttpReader *reader);
bool net_http_request_begin (NetHttpReader *reader, NetHttpRequest *request);
void net_http_request_end   (NetHttpReader *reader, NetHttpRequest *request);

#endif
