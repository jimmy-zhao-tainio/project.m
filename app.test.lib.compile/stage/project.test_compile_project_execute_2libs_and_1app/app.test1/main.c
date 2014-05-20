#include <lib.test1/test1.h>
#include <lib.test1/test2.h>
#include <lib.test2/test3.h>
#include <lib.test2/test4.h>

int main (void)
{
	return test1 () && test2 () && test3 () && test4 ();
}
