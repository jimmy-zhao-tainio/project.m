#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.core/string.h>
#include <lib.core/random.h>
#include <lib.encode/encode.h>
#include <stdio.h>

#include "test-encode.h"

bool test_encode_base64 (Test *test)
{
        char *base64;
        char bytes[1024 * 16];
        size_t i;

        TITLE ();
        CATCH (encode_base64 (NULL, 0));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 1);
        CATCH (encode_base64 ("", 0));
        CATCH (error_at (0).error != ErrorInvalidArgument);
        CATCH (error_at (0).code != 2);
        memory_commit_limit (0);
        CATCH (encode_base64 ("abc", 3));
        CATCH (error_at (0).error != ErrorFunctionCall);
        memory_commit_limit (ULLONG_MAX);
        CATCH (!(base64 = encode_base64 ("any carnal pleasure.", 20)));
        CATCH (!string_equals (base64, "YW55IGNhcm5hbCBwbGVhc3VyZS4="));
        string_destroy (base64);
        CATCH (!random_open ());
        for (i = 0; i < 1000; i++) {
                CATCH (!random_bytes ((unsigned char *)&bytes, 1024 * 16));
                CATCH (!(base64 = encode_base64 (bytes, 1024 * 16)));
                CATCH (string_length (base64) < 21848);
                string_destroy (base64);
        }
        random_close ();
        PASS ();
}
