#include <lib.core/file-reader.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

FileReader *file_reader_create (const char *path)
{
	FileReader *reader;
	struct stat st;

	if (!(reader = memory_create (sizeof (FileReader)))) {
		error (FunctionCall);
		return NULL;
	}
	reader->file_descriptor = -1;
	if (stat (path, &st) != 0) {
		file_reader_destroy (reader);
		error (SystemCall);
		return NULL;
	}
	reader->size = st.st_size;
	if ((reader->file_descriptor = open (path, O_RDONLY, 0)) == -1) {
		file_reader_destroy (reader);
		error (SystemCall);
		return NULL;
	}
	if (!(reader->map = (unsigned char *)mmap (NULL, (size_t)reader->size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, reader->file_descriptor, 0))) {
		file_reader_destroy (reader);
		error (SystemCall);
		return NULL;
	}
	return reader;
}

void file_reader_destroy (FileReader *reader)
{
	if (!reader) {
		return;
	}
	if (reader->map) {
		munmap (reader->map, (size_t)reader->size);
	}
	if (reader->file_descriptor != -1) {
		close (reader->file_descriptor);
	}
	memory_destroy (reader);
}
