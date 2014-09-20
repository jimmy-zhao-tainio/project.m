#include <lib.compile/compile-action.h>
#include <lib.compile/compile-print.h>
#include <lib.core/memory.h>
#include <lib.core/string.h>

CompileAction *compile_action_create (CompileActionType type)
{
	CompileAction *action;

	if (type != COMPILE_ACTION_CC &&
	    type != COMPILE_ACTION_ARCH &&
	    type != COMPILE_ACTION_INDEX &&
	    type != COMPILE_ACTION_BUILD) {
		compile_debug_invalid_arguments ();
		return NULL;
	}
	if (!(action = memory_create (sizeof (CompileAction)))) {
		compile_debug_allocate_memory ();
		return NULL;
	}
	action->type = type;
	action->print = NULL;
	action->command = NULL;
	return action;
}

void compile_action_destroy (CompileAction *action)
{
	if (!action) {
		return;
	}
	if (action->print) {
		string_destroy (action->print);
	}
	if (action->command) {
		string_destroy (action->command);
	}
	memory_destroy (action);
}
