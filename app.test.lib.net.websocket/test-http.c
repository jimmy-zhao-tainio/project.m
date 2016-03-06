#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/threads.h>
#include <lib.core/threads-signal.h>
#include <lib.net.websocket/websocket.h>
#include <unistd.h>
#include <stdio.h>

#include "test-http.h"

bool test_http_request_begin (Test *test)
{
        NetHttpReader reader;

        TITLE ();
        reader.buffer = "";
        reader.length = 0;
        reader.request_end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request_end != 0);

        reader.buffer = "\r";
        reader.length = 1;
        reader.request_end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request_end != 0);
        
        reader.buffer = "\r\n";
        reader.length = 2;
        reader.request_end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request_end != 0);

        reader.buffer = "\r\n\r";
        reader.length = 3;
        reader.request_end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request_end != 0);

        reader.buffer = "\r\n\r\n";
        reader.length = 4;
        reader.request_end = 0;
        CATCH (!net_http_request_begin (&reader));
        CATCH (reader.request_end != 4);
        
        reader.buffer = "\r\n\r\n ";
        reader.length = 5;
        reader.request_end = 0;
        CATCH (!net_http_request_begin (&reader));
        CATCH (reader.request_end != 4);

        reader.buffer = " \r\n\r\n ";
        reader.length = 6;
        reader.request_end = 0;
        CATCH (!net_http_request_begin (&reader));
        CATCH (reader.request_end != 5);

        reader.buffer = " \r\n\r";
        reader.length = 4;
        reader.request_end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request_end != 1);

        reader.buffer = "  \r\n\r";
        reader.length = 5;
        reader.request_end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request_end != 2);

        reader.buffer = "  \r\n ";
        reader.length = 5;
        reader.request_end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request_end != 2);

        reader.buffer = "1234567";
        reader.length = 7;
        reader.request_end = 0;
        CATCH (net_http_request_begin (&reader));
        CATCH (reader.request_end != 4);
        
        PASS ();
}
