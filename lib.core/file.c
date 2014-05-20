#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#include <lib.core/file.h>
#include <lib.core/file-private.h>
#include <lib.core/memory.h>
#include <lib.core/string.h>
#include <lib.core/error.h>

static void file_destroy (File *file);
static bool directory_read_try (Directory *directory, Directory **directory_next, File **file_next, DIR **dir, char **path, char **name);
static bool directory_read_recursive_try (Directory *directory);

typedef struct _FilePrivate FilePrivate;

struct _FilePrivate
{
	FILE *file;
};

#define FP(x) ((FilePrivate *)((x)->private))

static uint16_t path_size = FILE_PATH_SIZE;

char *directory_current_path (void)
{
	char *path;

    	if (!(path = string_create_with_size (path_size))) {
		error (FunctionCall);
        	return NULL;
	}
	if (!getcwd (path, path_size)) {
        	string_destroy (path);
		error (SystemCall);
        	return NULL;
	}
	return path;
}

bool file_path_is_valid (const char *path)
{
	size_t length;
	size_t i = 0;

	if (!path) {
		return false;
	}
	length = string_length (path);
	if (length >= path_size) {
		return false;
	}
	if (path[0] == '/' &&
	    path[1] == '\0') {
		return true;
	}
	while (1) {
		if (path[i] != '/') {
			return false;
		}
		i++;
		if (path[i] == '/' ||
		    path[i] == '\0') {
			return false;
		}
		i++;
		while (path[i] != '/' &&
		       path[i] != '\0') {
			i++;
		}
		if (path[i] == '\0') {
			return true;
		}
	}
}

char *file_name_from_path (const char *path)
{
	char *name;
	size_t length;
    
	if (!file_path_is_valid (path)) {
		error (InvalidOperation);
		return NULL;
	}
	if (string_equals (path, "/")) {
		if (!(name = string_create (path))) {
			error (FunctionCall);
		}
		return name;
	}
	for (length = string_length (path);
	     length > 0 && path[length - 1] != '/';
	     length--);
	if (!(name = string_create (path + length))) {
		error (FunctionCall);
	}
	return name;
}

char *directory_sub_path (const char *path)
{
	char *sub;
	size_t i = 0;

	if (!file_path_is_valid (path)) {
		error (InvalidOperation);
		return NULL;
	}
	while (path[i] != '\0') {
		i++;
	}
	while (path[i] != '/') {
		i--;
	}
	if (i == 0) {
		i++;
	}
	if (!(sub = string_create_with_size (i + 1))) {
		error (FunctionCall);
		return NULL;
	}
	memcpy (sub, path, i);
	sub[i] = '\0';
	return sub;
}

Directory *directory_open (const char *path)
{
	Directory *directory = NULL;
	struct stat file_stat;
	
	if (!file_path_is_valid (path)) {
		error (InvalidOperation);
		return NULL;
	}
	if (stat (path, &file_stat) == -1) {
		error (SystemCall);
		return NULL;
	}
	if (!(directory = memory_create (sizeof (Directory)))) {
		error (FunctionCall);
        	return NULL;
	}
	directory->object.id = object_id ();
	if (!(directory->path = string_create (path))) {
		memory_destroy (directory);
		error (FunctionCall);
		return NULL;
	}
	if (!(directory->name = file_name_from_path (path))) {
		string_destroy (directory->path);
		memory_destroy (directory);
		error (FunctionCall);
		return NULL;
	}
	directory->directories = NULL;
	directory->files = NULL;
	directory->modified = file_stat.st_mtime;
	return directory;
}

Directory *directory_open_current (void)
{
	Directory *directory;
	char *path;

	if (!(path = directory_current_path ())) {
		error (FunctionCall);
		return NULL;
	}
	directory = directory_open (path);
	string_destroy (path);
	return directory;
}

void directory_close (Directory *directory)
{
	ListNode *node;

	if (!directory) {
		error (InvalidArgument);
        	return;
	}
	for (node = list_first (directory->directories); node; node = node->next) {
		directory_close (node->data);
	}
	for (node = list_first (directory->files); node; node = node->next) {
		file_destroy (node->data);
	}
	if (directory->path) {
		string_destroy (directory->path);
	}
	if (directory->name) {
		string_destroy (directory->name);
	}
	list_destroy (directory->directories);
	list_destroy (directory->files);
	memory_destroy (directory);
}

bool directory_read (Directory *directory)
{
	Directory *directory_next = NULL;
	File *file_next = NULL;
	ListNode *node;
	DIR *dir = NULL;
	char *path = NULL;
	char *name = NULL;

	if (!directory) {
		error (InvalidArgument);
		return false;
	}
	if (directory->directories ||
	    directory->files) {
		return true;
	}
	if (!directory_read_try (directory, &directory_next, &file_next, &dir, &path, &name)) {
		if (directory_next) {
			directory_close (directory_next);
		}
		if (file_next) {
			file_destroy (file_next);
		}
		for (node = list_first (directory->directories); node; node = node->next) {
			directory_close (node->data);
		}
		list_destroy (directory->directories);
		directory->directories = NULL;
		for (node = list_first (directory->files); node; node = node->next) {
			file_destroy (node->data);
		}
		list_destroy (directory->files);
		directory->files = NULL;
		if (dir) {
			closedir (dir);
		}
		if (path) {
			string_destroy (path);
		}
		if (name) {
			string_destroy (name);
		}
		return false;
	}
	return true;
}

bool directory_read_recursive (Directory *directory)
{
	if (!directory_read_recursive_try (directory)) {
		directory_close (directory);
		return false;
	}
	return true;
}

File *directory_find_file (Directory *directory, const char *name)
{
	ListNode *node;
	File *found;
	
	if (!directory || !name) {
		error (InvalidArgument);
		return NULL;
	}
	for (node = list_first (directory->files); node; node = node->next) {
		found = node->data;
		if (string_equals (found->name, name)) {
			return found;
		}
	}
	return NULL;
}

Directory *directory_find_directory (Directory *directory, const char *name)
{
	ListNode *node;
	Directory *found;
	
	if (!directory || !name) {
		error (InvalidArgument);
		return NULL;
	}
	for (node = list_first (directory->directories); node; node = node->next) {
		found = node->data;
		if (string_equals (found->name, name)) {
			return found;
		}
	}
	return NULL;
}

File *directory_add_fake_file (Directory *directory, const char *name)
{
	File *file;

	if (!directory || !name) {
		error (InvalidArgument);
		return NULL;
	}
	if (!(file = memory_create (sizeof (File)))) {
		error (FunctionCall);
		return NULL;
	}
	file->object.id = object_id ();
	if (!(file->path = string_create (directory->path)) ||
	    !string_append (&file->path, "/") ||
	    !string_append (&file->path, name)) {
		file_destroy (file);
		error (FunctionCall);
		return NULL;
	}
	if (!(file->name = file_name_from_path (file->path))) {
		file_destroy (file);
		error (FunctionCall);
		return NULL;
	}
	file->modified = 0;
	if (!list_append (directory->files, file)) {
		file_destroy (file);
		error (FunctionCall);
		return NULL;
	}
	return file;
}

bool file_open (File *file)
{
	if (!file) {
		error (InvalidArgument);
		return false;
	}
	if (file->private) {
		error (InvalidArgument);
		return false;
	}
	if (!(file->private = memory_create (sizeof (FilePrivate)))) {
		error (FunctionCall);
		return false;
	}
	FP (file)->file = fopen (file->path, "r");
	if (!FP (file)->file) {
		memory_destroy (file->private);
		file->private = NULL;
		error (SystemCall);
		return false;
	}
	return true;
}

bool file_readline (File *file, char *line, size_t *bytes_read)
{
	int c;
	size_t max;
	
	if (!file ||
	    !line ||
	    (max = memory_size (line)) == 0 ||
	    !bytes_read ||
	    !FP (file)) {
		error (InvalidArgument);
		return false;
	}
	if (!size_t_add (max, 1, NULL)) {
		error (Overflow);
		return false;
	}
	*bytes_read = 0;
	while (1) {
		c = fgetc (FP (file)->file);
		if (c == EOF) {
			line[*bytes_read] = '\0';
			return true;
		}
		if ((unsigned char)c == '\n') {
			line[*bytes_read] = '\0';
			*bytes_read += 1;
			return true;
		}
		if (*bytes_read + 1 < max) {
			line[*bytes_read] = (char)c;
			*bytes_read += 1;
		}
		else
		{
			line[*bytes_read] = '\0';
			*bytes_read += 1;
			do
			{
				c = fgetc (FP (file)->file);
			} while (c != EOF && c != '\n');
			return true;
		}
	}
}

void file_close (File *file)
{
	if (!file) {
		error (InvalidArgument);
		return;
	}
	if (!file->private) {
		return;
	}
	if (FP (file)->file) {
		fclose (FP (file)->file);
	}
	memory_destroy (file->private);
	file->private = NULL;
}

bool file_remove (const char *path)
{
	if (!file_path_is_valid (path)) {
		error (InvalidOperation);
		return false;
	}
	if (remove (path) != 0) {
		error (SystemCall);
		return false;
	}
	return true;
}

bool file_exists (const char *path)
{
	struct stat buf;

	if (!file_path_is_valid (path)) {
		error (InvalidOperation);
		return false;
	}
	if (!stat (path, &buf)) {
		return true;
	}
	return false;
}

void file_path_size (uint16_t size)
{
	if (size == 0) {
		size = FILE_PATH_SIZE;
	}
	path_size = size;
}

static bool directory_read_try (Directory *directory, Directory **directory_next, File **file_next, DIR **dir, char **path, char **name)
{
	struct dirent *dir_entry;
	struct stat file_stat;
	time_t modified;

	if (directory->directories ||
	    directory->files) {
		error (InvalidArgument);
		return true;
	}
	if (!(*dir = opendir (directory->path))) {
		error (SystemCall);
		return false;
	}
	if (!(directory->directories = list_create ())) {
		error (FunctionCall);
		return false;
	}
	if (!(directory->files = list_create ())) {
		error (FunctionCall);
		return false;
	}
	while ((dir_entry = readdir (*dir))) {
		if (strlen (dir_entry->d_name) == 1 &&
		    dir_entry->d_name[0] == '.') {
			continue;
		}
		if (strlen (dir_entry->d_name) == 2 &&
		    dir_entry->d_name[0] == '.' &&
		    dir_entry->d_name[1] == '.') {
			continue;
		}
		if (!(*path = string_create (directory->path)) ||
		    !string_append (path, "/") ||
		    !string_append (path, dir_entry->d_name)) {
			error (FunctionCall);
			return false;
		}
		if (!(*name = file_name_from_path (*path))) {
			error (FunctionCall);
			return false;
		}
		if (lstat (*path, &file_stat) != 0) {
			string_destroy (*path);
			string_destroy (*name);
			*path = NULL;
			*name = NULL;
			error (SystemCall);
			continue;
		}
		modified = file_stat.st_mtime;
		if (S_ISDIR (file_stat.st_mode) != 0) {
			if (!(*directory_next = memory_create (sizeof (Directory)))) {
				error (FunctionCall);
				return false;
			}
			(*directory_next)->object.id = object_id ();
			if (!list_insert (directory->directories, *directory_next)) {
				error (FunctionCall);
				return false;
			}
			(*directory_next)->path = *path;
			(*directory_next)->name = *name;
			(*directory_next)->modified = modified;
			*path = NULL;
			*name = NULL;
			*directory_next = NULL;
		} 
		else if (S_ISREG (file_stat.st_mode) != 0) {
			if (!(*file_next = memory_create (sizeof (File)))) {
				error (FunctionCall);
				return false;
			}
			(*file_next)->object.id = object_id ();
			if (!list_insert (directory->files, *file_next)) {
				error (FunctionCall);
				return false;
			}
			(*file_next)->path = *path;
			(*file_next)->name = *name;
			(*file_next)->modified = modified;
			*path = NULL;
			*name = NULL;
			*file_next = NULL;
		}
		else {
			string_destroy (*path);
			string_destroy (*name);
			*path = NULL;
			*name = NULL;
		}
	}
	closedir (*dir);
	return true;
}

static bool directory_read_recursive_try (Directory *directory)
{
	ListNode *node;

	if (!directory_read (directory)) {
		error (FunctionCall);
		return false;
	}
	for (node = list_first (directory->directories); node; node = node->next) {
		if (!directory_read_recursive (node->data)) {
			error (FunctionCall);
			return false;
		}
	}
	return true;
}

static void file_destroy (File *file)
{
	if (!file) {
		error (InvalidArgument);
		return;
	}
	file_close (file);
	if (file->path) {
		string_destroy (file->path);
	}
	if (file->name) {
		string_destroy (file->name);
	}
	memory_destroy (file);
}
