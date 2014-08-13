#ifndef file_h
#define file_h

#include <lib.core/defines.h>
#include <lib.core/object.h>
#include <lib.core/list.h>

typedef struct _Directory Directory;
typedef struct _File File;

struct _Directory
{
	Object object;

	char *path;
	char *name;
	time_t modified;
	List *directories;
	List *files;
};

struct _File
{
	Object object;

	char *path;
	char *name;
	time_t modified;
	void *private;
};

char *directory_current_path        (void);
char *directory_current_path_append (const char *partial_path);
char *directory_sub_path            (const char *path);

Directory *directory_open         (const char *path);
Directory *directory_open_current (void);
void       directory_close        (Directory *directory);

bool directory_read           (Directory *directory);
bool directory_read_recursive (Directory *directory);

File      *directory_find_file      (Directory *directory, const char *name);
Directory *directory_find_directory (Directory *directory, const char *name);
File      *directory_add_fake_file  (Directory *directory, const char *name);

bool  file_open           (File *file);
bool  file_readline       (File *file, char *line, size_t *bytes_read);
void  file_close          (File *file);
bool  file_remove         (const char *path);
bool  file_exists         (const char *path);
char *file_name_from_path (const char *path);
bool  file_path_is_valid  (const char *path);

#endif
