#include <stdio.h>
#include <lib.core/defines.h>
#include <lib.core/file.h>
#include <lib.core/list.h>
#include <lib.core/string.h>
#include <lib.core/memory.h>

typedef struct _StackLevel StackLevel;

struct _StackLevel 
{
	Directory *directory;
	size_t level;
};

int main (void)
{
	Directory *directory = NULL;
	Directory *sub_directory = NULL;
	Directory *sub_sub_directory = NULL;
	StackLevel *stack_level = NULL;
	File *sub_file = NULL;
	ListNode *node = NULL;
	List *stack = NULL;
	char *path = NULL;
	char *line = NULL;
	size_t bytes_read;
	size_t level;
	size_t path_index;

	if (!(stack = list_create ())) {
		printf ("Failed to allocate memory.\n");
		goto exit;
	}
	if (!(path = directory_current_path ())) {
		printf ("Failed to get current path.\n");
		goto exit;
	}
	if (!(directory = directory_open (path))) {
		printf ("Failed to open directory.\n");
		goto exit;
	}
	if (!directory_read_recursive (directory)) {
		printf ("Failed to read directory.\n");
		goto exit;
	}
	if (!(stack_level = memory_create (sizeof (StackLevel)))) {
		printf ("Failed to allocate memory.\n");
		goto exit;
	}
	if (!(line = string_create_with_size (1024))) {
		printf ("Failed to allocate memory.\n");
		goto exit;
	}
	path_index = string_length (directory->path) - string_length (directory->name);
	stack_level->directory = directory;
	stack_level->level = 0;
	if (!list_insert (stack, stack_level)) {
		printf ("Failed to allocate memory.\n");
		goto exit;
	}
	while (list_count (stack) != 0) {
		node = list_first (stack);
		stack_level = node->data;
		sub_directory = stack_level->directory;
		level = stack_level->level;
		list_remove (stack, node);
		memory_destroy (stack_level);
		printf ("d %s\n", sub_directory->path + path_index);
		for (node = list_first (sub_directory->files); node; node = node->next) {
			sub_file = node->data;
			printf ("f %s\n", sub_file->name);
			if (!file_open (sub_file)) {
				printf ("Failed to open: %s\n", sub_file->path);
				goto exit;
			}
			while (file_readline (sub_file, line, &bytes_read)) {
				if (bytes_read == 0) {
					break;
				}
				/*if (!string_escape_c (&line)) {
					printf ("Failed to allocate memory.\n");
					goto exit;
				}*/
				printf ("+ %s\n", line);
			}
			file_close (sub_file);
		}
		for (node = list_first (sub_directory->directories); node; node = node->next) {
			sub_sub_directory = node->data;
			if (!(stack_level = memory_create (sizeof (StackLevel)))) {
				printf ("Failed to allocate memory.\n");
				goto exit;
			}
			stack_level->directory = sub_sub_directory;
			stack_level->level = level + 1;
			if (!list_insert (stack, stack_level)) {
				printf ("Failed to allocate memory.\n");
				goto exit;
			}
		}
	}
exit:
	if (path) {
		string_destroy (path);
	}
	if (directory) {
		directory_close (directory);
	}
	if (stack) {
		for (node = list_first (stack); node; node = node->next) {
			memory_destroy (node->data);
		}
		list_destroy (stack);
	}
	return 0;
}
