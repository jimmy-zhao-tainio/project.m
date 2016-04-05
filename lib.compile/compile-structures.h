#ifndef compile_structures_h
#define compile_structures_h

#include <lib.core/defines.h>
#include <lib.core/object.h>
#include <lib.core/file.h>
#include <lib.core/tree.h>
#include <lib.core/topological.h>
#include <lib.core/file.h>

typedef struct _Compile        Compile;
typedef struct _CompileAction  CompileAction;
typedef struct _CompileProject CompileProject;

typedef enum
{
	COMPILE_ACTION_CC,
	COMPILE_ACTION_ARCH,
	COMPILE_ACTION_INDEX,
	COMPILE_ACTION_BUILD,
	COMPILE_ACTION_SILENT
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

struct _CompileProject
{
	Directory *directory;
	/* List of compile nodes */
	List *nodes;
	/* Compile nodes in topological structure. */
	Topological *topological;
	/* Maps apps and libs directories to their Compile node. */
	Tree *directory_to_compile;
	/* Sorted order of Compile nodes, set by compile_project_prepare if successful. */
	List *sorted;
};

#endif
