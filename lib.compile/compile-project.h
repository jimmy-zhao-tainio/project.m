#ifndef compile_project_h
#define compile_project_h

#include <lib.core/file.h>
#include <lib.core/topological.h>
#include <lib.core/tree.h>

typedef struct _CompileProject CompileProject;

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

CompileProject *compile_project_create (const char *path);
bool compile_project_prepare (CompileProject *project);
bool compile_project_execute (CompileProject *project, bool bootstrap);
bool compile_project_execute_with_directory_name (CompileProject *project, const char *directory_name, bool bootstrap);
void compile_project_destroy (CompileProject *project);

#endif
