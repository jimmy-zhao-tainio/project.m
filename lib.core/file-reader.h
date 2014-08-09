#ifndef file_reader_h
#define file_reader_h

#include <lib.core/defines.h>
#include <stdio.h>

typedef struct s_FileReader FileReader;

struct s_FileReader
{
	int file_descriptor;
	off_t size;
	unsigned char *map;
};

FileReader *file_reader_create (const char *path);
void file_reader_destroy (FileReader *reader);

#endif
