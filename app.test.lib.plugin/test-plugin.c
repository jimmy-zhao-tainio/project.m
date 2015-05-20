#include <lib.core/error.h>
#include <lib.core/memory.h>
#include <lib.plugin/plugin.h>

#include "test-plugin.h"

bool test_plugin_create_invalid_argument (Test *test)
{
        TITLE ();
        CATCH (plugin_create (NULL));
        CATCH (error_count () != 1);
        CATCH (error_at (0).error != ErrorInvalidArgument);
        PASS ();
}

bool test_plugin_create_function_call_1 (Test *test)
{
        TITLE ();
        memory_commit_limit (0);
        CATCH (plugin_create ("2"));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 1);
        PASS ();
}

bool test_plugin_create_function_call_2 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (Plugin));
        CATCH (plugin_create ("2"));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 2);
        PASS ();
}

bool test_plugin_create_function_call_3 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (Plugin) +
                             sizeof (size_t) + 2);
        CATCH (plugin_create ("2"));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 3);
        PASS ();
}

bool test_plugin_create_function_call_4 (Test *test)
{
        TITLE ();
        memory_commit_limit (sizeof (size_t) + sizeof (Plugin) +
                             2 * (sizeof (size_t) + 3));
        CATCH (plugin_create ("/3"));
        CATCH (error_at (0).error != ErrorFunctionCall);
        CATCH (error_at (0).code != 4);
        PASS ();
}
