#ifndef error_h
#define error_h

#include <lib.core/defines.h>

#define ERROR_ITEMS_MAX 64

typedef enum {
#undef ERROR
#define ERROR(e) Error##e,
#include <lib.core/error-enums.h>
#include <lib.core/error-enums-pattern.h>
#include <lib.core/error-enums-app.h>
	ErrorNull
#undef ERROR
} Error;

typedef struct {
	const char *file;
	const char *function;
	int line;
	int code;
	Error error;
	const char *literal;
	int invalid_error;
} ErrorItem;

void      error_add (const char *file, const char *function, int line, int code, Error error);
void      error_reset (void);
size_t    error_count (void);
ErrorItem error_at (size_t index);
void      error_silent (bool state);

#define error(e) error_add (__FILE__, __func__, __LINE__, 0, Error##e)
#define error_code(e,c) error_add (__FILE__, __func__, __LINE__, c, Error##e)

#endif
