#include <stdio.h>
#include <lib.test/test.h>

#include "test-http.h"
#include "test-frame.h"
#include "test-websocket.h"
#include "test-websocket-connect-error.h"
#include "test-websocket-add-error-1.h"
#include "test-websocket-add-error-2.h"
#include "test-websocket-add.h"
#include "test-websocket-http-on-read-1.h"
#include "test-websocket-http-on-read-2.h"
#include "test-websocket-http-on-read-3.h"
#include "test-websocket-upgrade-1.h"
#include "test-websocket-upgrade-2.h"
#include "test-websocket-upgrade-3.h"
#include "test-websocket-upgrade-4.h"
#include "test-websocket-upgrade-5.h"
#include "test-websocket-upgrade.h"

bool (*tests[]) (Test *test) =
{
        /* OBS! valgrind will show openssl allocations which aren't 
         * cleaned up. 
         */
        &test_http_request_begin,
        &test_http_request_end_1,
        &test_http_request_end_2,
        &test_http_set_method,
        &test_http_set_uri,
        &test_http_set_version,
        &test_http_set_headers,
        &test_http_get_header_1,
        &test_http_get_header_2,
        &test_frame_create_function_call,
        &test_frame_create,
        &test_frame_append_invalid_operation,
        &test_frame_append_function_call,
        &test_frame_append,
        &test_websocket_create_invalid_argument_1,
        &test_websocket_create_invalid_argument_2,
        &test_websocket_create_invalid_argument_3,
        &test_websocket_create_function_call_1,
        &test_websocket_create_function_call_2,
        &test_websocket_create_function_call_3,
        &test_websocket_create,
        &test_websocket_connect_error,
        &test_websocket_add_error_1,
        &test_websocket_add_error_2,
        &test_websocket_add,
        &test_websocket_http_on_read_1,
        &test_websocket_http_on_read_2,
        &test_websocket_http_on_read_3,
        &test_websocket_upgrade_1,
        &test_websocket_upgrade_2,
        &test_websocket_upgrade_3,
        &test_websocket_upgrade_4,
        &test_websocket_upgrade_5,
        &test_websocket_upgrade,
	NULL
};

int main (int argc, char **argv)
{
	return test_array (argc, argv, tests) ? EXIT_SUCCESS : EXIT_FAILURE;
}
