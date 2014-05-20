#ifndef compile_h
#define compile_h

#include <lib.core/defines.h>
#include <lib.core/object.h>
#include <lib.core/file.h>
#include <lib.core/tree.h>
#include <lib.core/topological.h>

typedef struct _Compile       Compile;
typedef struct _CompileAction CompileAction;

typedef enum
{
	COMPILE_ACTION_CC,
	COMPILE_ACTION_ARCH,
	COMPILE_ACTION_INDEX,
	COMPILE_ACTION_BUILD
} CompileActionType;

struct _Compile
{
	Object object;
	/* Project directory. */
	Directory *project;
	/* app or lib directory to compile. */
	Directory *directory;
	/* Libraries (Directory *) nodes that are included directly or indirectly by this app or lib.
	 * OBS: A self balanced binary tree is not needed at all here, we would only need 
	 * a collection with unique items in no particular order. The topological order is stored in
	 * libraries_sorted.
	 */
	Tree *libraries;
	/* Sorted libraries */
	List *libraries_sorted;
	/* c and h files (File *) nodes in this app or lib. */
	Topological *files;
	/* Sorted File nodes, set by compile_prepare if successful. */
	List *sorted;
	/* List of .o-files to link */
	List *o_files;
	/* List of actions to execute. */
	List *actions;
};

struct _CompileAction
{
	CompileActionType type;
	char *print;
	char *command;
};

Compile *compile_create (Directory *project, Directory *directory);
bool compile_prepare (Compile *compile);
bool compile_actions (Compile *compile, const char *project_path);
bool compile_execute (Compile *compile, bool bootstrap);
void compile_destroy (Compile *compile);

#endif
