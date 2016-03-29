#ifndef lib_net_http_h
#define lib_net_http_h

typedef struct _NetHttpReader  NetHttpReader;
typedef struct _NetHttpRequest NetHttpRequest;

struct _NetHttpRequest {
        size_t end;
        bool method_get;
        bool method_head;
        size_t method_length;
        size_t uri_begin;
        size_t uri_length;
        size_t version_begin;
        size_t version_length;
        size_t headers_begin;
        size_t headers_length;
        size_t headers_count;
};

struct _NetHttpReader {
        char *buffer;
        size_t size;
        size_t length;
        bool initialized;
        NetHttpRequest request;
};

bool net_http_reader_create  (NetHttpReader *reader);
bool net_http_reader_append  (NetHttpReader *reader, char *buffer, size_t length);
void net_http_reader_destroy (NetHttpReader *reader);
bool net_http_request_begin  (NetHttpReader *reader);
void net_http_request_end    (NetHttpReader *reader);
bool net_http_set_method     (NetHttpReader *reader);
bool net_http_set_uri        (NetHttpReader *reader);
bool net_http_set_version    (NetHttpReader *reader);
bool net_http_set_headers    (NetHttpReader *reader);
char *net_http_get_header    (NetHttpReader *reader, char *header);

#endif
