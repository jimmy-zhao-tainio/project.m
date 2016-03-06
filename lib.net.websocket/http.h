#ifndef lib_net_http_h
#define lib_net_http_h

typedef struct _NetHttpReader NetHttpReader;

struct _NetHttpReader {
        char *buffer;
        size_t size;
        size_t length;
        bool initialized;
        size_t request_end;
};

bool net_http_reader_create  (NetHttpReader *reader);
bool net_http_reader_append  (NetHttpReader *reader, char *buffer, size_t length);
void net_http_reader_destroy (NetHttpReader *reader);
bool net_http_request_begin  (NetHttpReader *reader);
void net_http_request_end    (NetHttpReader *reader);

#endif
