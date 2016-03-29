#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.websocket/websocket.h>
#include <lib.core/string.h>
#include <unistd.h>
#include <stdio.h>

#include "test-http.h"

bool test_http_request_begin (Test *test)
{
        NetHttpReader reader;

        TITLE ();
        reader.buffer = "";
        reader.length = 0;
        reader.request.end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request.end != 0);

        reader.buffer = "\r";
        reader.length = 1;
        reader.request.end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request.end != 0);
        
        reader.buffer = "\r\n";
        reader.length = 2;
        reader.request.end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request.end != 0);

        reader.buffer = "\r\n\r";
        reader.length = 3;
        reader.request.end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request.end != 0);

        reader.buffer = "\r\n\r\n";
        reader.length = 4;
        reader.request.end = 0;
        CATCH (!net_http_request_begin (&reader));
        CATCH (reader.request.end != 4);
        
        reader.buffer = "\r\n\r\n ";
        reader.length = 5;
        reader.request.end = 0;
        CATCH (!net_http_request_begin (&reader));
        CATCH (reader.request.end != 4);

        reader.buffer = " \r\n\r\n ";
        reader.length = 6;
        reader.request.end = 0;
        CATCH (!net_http_request_begin (&reader));
        CATCH (reader.request.end != 5);

        reader.buffer = " \r\n\r";
        reader.length = 4;
        reader.request.end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request.end != 1);

        reader.buffer = "  \r\n\r";
        reader.length = 5;
        reader.request.end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request.end != 2);

        reader.buffer = "  \r\n ";
        reader.length = 5;
        reader.request.end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request.end != 2);

        reader.buffer = "1234567";
        reader.length = 7;
        reader.request.end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request.end != 4);
        
        PASS ();
}

bool test_http_request_end_1 (Test *test)
{
        NetHttpReader reader;

        TITLE ();
        reader.buffer = string_create (" \r\n\r\nx");
        reader.length = 6;
        reader.request.end = 0;
        CATCH (!net_http_request_begin (&reader));
        CATCH (reader.request.end != 5);
        net_http_request_end (&reader);
        CATCH (reader.length != 1);
        CATCH (reader.buffer[0] != 'x');
        CATCH (reader.request.end != 0);
        string_destroy (reader.buffer);
        PASS ();
}

bool test_http_request_end_2 (Test *test)
{
        NetHttpReader reader;

        TITLE ();
        reader.buffer = string_create (" \r\n\r\n");
        reader.length = 5;
        reader.request.end = 0;
        CATCH (!net_http_request_begin (&reader));
        CATCH (reader.request.end != 5);
        net_http_request_end (&reader);
        CATCH (reader.length != 0);
        CATCH (reader.request.end != 0);
        string_destroy (reader.buffer);
        PASS ();
}

bool test_http_set_method (Test *test)
{
        NetHttpReader reader;

        TITLE ();
        reader.buffer = "GET ";
        reader.length = string_length (reader.buffer);
        CATCH (!net_http_set_method (&reader));
        CATCH (reader.request.method_get != true);
        CATCH (reader.request.method_length != 3);
        CATCH (reader.request.method_head != false);
        
        reader.buffer = "HEAD ";
        reader.length = string_length (reader.buffer);
        CATCH (!net_http_set_method (&reader));
        CATCH (reader.request.method_head != true);
        CATCH (reader.request.method_length != 4);
        CATCH (reader.request.method_get != false);
        
        reader.buffer = "GET";
        reader.length = string_length (reader.buffer);
        CATCH (net_http_set_method (&reader));
        CATCH (reader.request.method_get != false);
        CATCH (reader.request.method_head != false);
        CATCH (reader.request.method_length != 0);
        
        PASS ();
}

bool test_http_set_uri (Test *test)
{
        NetHttpReader reader;

        TITLE ();
        reader.buffer = "GET ";
        reader.length = string_length (reader.buffer);
        CATCH (!net_http_set_method (&reader));
        CATCH (net_http_set_uri (&reader));

        reader.buffer = "GET  ";
        reader.length = string_length (reader.buffer);
        CATCH (!net_http_set_method (&reader));
        CATCH (net_http_set_uri (&reader));

        reader.buffer = "GET   ";
        reader.length = string_length (reader.buffer);
        CATCH (!net_http_set_method (&reader));
        CATCH (net_http_set_uri (&reader));

        reader.buffer = "GET *";
        reader.length = string_length (reader.buffer);
        CATCH (!net_http_set_method (&reader));
        CATCH (net_http_set_uri (&reader));

        reader.buffer = "GET * ";
        reader.length = string_length (reader.buffer);
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (reader.request.uri_begin != 4);
        CATCH (reader.request.uri_length != 1);

        reader.buffer = "GET index.html ";
        reader.length = string_length (reader.buffer);
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (reader.request.uri_begin != 4);
        CATCH (reader.request.uri_length != 10);

        PASS ();
}

bool test_http_set_version (Test *test)
{
        NetHttpReader reader;

        TITLE ();
        reader.buffer = "GET * ";
        reader.length = string_length (reader.buffer);
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (net_http_set_version (&reader));

        reader.buffer = "GET * HTTP/X.X\r";
        reader.length = string_length (reader.buffer);
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (net_http_set_version (&reader));

        reader.buffer = "GET * HTTP/X.X\r\n";
        reader.length = string_length (reader.buffer);
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (!net_http_set_version (&reader));
        CATCH (reader.request.version_begin != 6);
        CATCH (reader.request.version_length != 8);
        PASS ();
}

bool test_http_set_headers (Test *test)
{
        NetHttpReader reader;

        TITLE ();
        reader.buffer = "GET * HTTP/X.X\r\n";
        reader.length = string_length (reader.buffer);
        reader.request.end = reader.length;
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (!net_http_set_version (&reader));
        CATCH (net_http_set_headers (&reader));

        reader.buffer = "GET * HTTP/X.X\r\n\r";
        reader.length = string_length (reader.buffer);
        reader.request.end = reader.length;
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (!net_http_set_version (&reader));
        CATCH (net_http_set_headers (&reader));

        reader.buffer = "GET * HTTP/X.X\r\nx\r\n";
        reader.length = string_length (reader.buffer);
        reader.request.end = reader.length;
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (!net_http_set_version (&reader));
        CATCH (net_http_set_headers (&reader));

        reader.buffer = "GET * HTTP/X.X\r\n:";
        reader.length = string_length (reader.buffer);
        reader.request.end = reader.length;
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (!net_http_set_version (&reader));
        CATCH (net_http_set_headers (&reader));

        reader.buffer = "GET * HTTP/X.X\r\nx:y";
        reader.length = string_length (reader.buffer);
        reader.request.end = reader.length;
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (!net_http_set_version (&reader));
        CATCH (net_http_set_headers (&reader));

        reader.buffer = "GET * HTTP/X.X\r\nx:y\r\n\r";
        reader.length = string_length (reader.buffer);
        reader.request.end = reader.length;
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (!net_http_set_version (&reader));
        CATCH (net_http_set_headers (&reader));

        reader.buffer = "GET * HTTP/X.X\r\n\r\n";
        reader.length = string_length (reader.buffer);
        reader.request.end = reader.length;
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (!net_http_set_version (&reader));
        CATCH (!net_http_set_headers (&reader));
        CATCH (reader.request.headers_begin != 16);
        CATCH (reader.request.headers_length != 2);
        CATCH (reader.request.headers_count != 0);

        reader.buffer = "GET * HTTP/X.X\r\nabc:def\r\n\r\n";
        reader.length = string_length (reader.buffer);
        reader.request.end = reader.length;
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (!net_http_set_version (&reader));
        CATCH (!net_http_set_headers (&reader));
        CATCH (reader.request.headers_begin != 16);
        CATCH (reader.request.headers_length != 11);
        CATCH (reader.request.headers_count != 1);

        reader.buffer = "GET * HTTP/X.X\r\nabc:def\r\na\r\n";
        reader.length = string_length (reader.buffer);
        reader.request.end = reader.length;
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (!net_http_set_version (&reader));
        CATCH (net_http_set_headers (&reader));

        reader.buffer = "GET * HTTP/X.X\r\nabc:def\r\nghi:jkl\r\n\r\n";
        reader.length = string_length (reader.buffer);
        reader.request.end = reader.length;
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (!net_http_set_version (&reader));
        CATCH (!net_http_set_headers (&reader));
        CATCH (reader.request.headers_begin != 16);
        CATCH (reader.request.headers_length != 20);
        CATCH (reader.request.headers_count != 2);

        PASS ();
}

bool test_http_get_header_1 (Test *test)
{
        NetHttpReader reader;
        char *value;

        TITLE ();
        reader.buffer = "GET * HTTP/X.X\r\na:bc\r\n\r\n";
        reader.length = string_length (reader.buffer);
        reader.request.end = reader.length;
        CATCH (!net_http_set_method (&reader));
        CATCH (!net_http_set_uri (&reader));
        CATCH (!net_http_set_version (&reader));
        CATCH (!net_http_set_headers (&reader));
        CATCH (!(value = net_http_get_header (&reader, "a")));
        CATCH (memory_size (value) != 3);
        CATCH (value[0] != 'b');
        CATCH (value[1] != 'c');
        CATCH (value[2] != '\0');
        memory_destroy (value);
        PASS ();
}

bool test_http_get_header_2 (Test *test)
{
        NetHttpReader reader;
        char *value;

        TITLE ();
        reader.buffer = "a:b\r";
        reader.length = string_length (reader.buffer);
        reader.request.end = reader.length;
        reader.request.headers_begin = 0;
        reader.request.headers_length = 4;
        CATCH (!(value = net_http_get_header (&reader, "a")));
        CATCH (memory_size (value) != 2);
        CATCH (value[0] != 'b');
        CATCH (value[1] != '\0');
        memory_destroy (value);
        PASS ();
}
