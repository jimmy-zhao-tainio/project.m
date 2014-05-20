#include <lib.core/defines.h>
#include <lib.core/error.h>
#include <lib.core/object.h>
#include <lib.core/object-private.h>

#include "test-object.h"

bool test_object_id_overflow (Test *test)
{
	TITLE ();
	object_id_set_counter (ULLONG_MAX - 1);
	CATCH (object_id () != ULLONG_MAX - 1);
	CATCH (object_id () != ULLONG_MAX);
	CATCH (error_count () == 0);
	CATCH (error_at (0).error != ErrorOverflow);
	CATCH (object_id () != 0);
	CATCH (object_id () != 1);
	PASS ();
}

bool test_object_id (Test *test)
{
	TITLE ();
	CATCH (object_id () != 0);
	CATCH (object_id () != 1);
	CATCH (object_id () != 2);
	PASS ();
}

bool test_object_id_set_counter (Test *test)
{
	TITLE ();
	object_id_set_counter (123);
	CATCH (object_id () != 123);
	object_id_set_counter (0);
	CATCH (object_id () != 0);
	object_id_set_counter (ULLONG_MAX);
	CATCH (object_id () != ULLONG_MAX);
	error_reset ();
	PASS ();
}
