#include <lib.encode/encode.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.core/string.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

char *encode_base64 (const char *buffer, size_t length)
{
        BIO *s_mem;
        BIO *f_base64;
	BUF_MEM *pointer;
        char *encoded;

        if (!buffer) {
                error_code (InvalidArgument, 1);
                return NULL;
        }
        if (length == 0) {
                error_code (InvalidArgument, 2);
                return NULL;
        }
	if (!(f_base64 = BIO_new (BIO_f_base64()))) {
                error_code (SystemCall, 1);
                return NULL;
        }
	if (!(s_mem = BIO_new (BIO_s_mem()))) {
                BIO_vfree (f_base64);
                error_code (SystemCall, 2);
                return NULL;
        }
	s_mem = BIO_push (f_base64, s_mem);
	BIO_set_flags (s_mem, BIO_FLAGS_BASE64_NO_NL); 
	if (BIO_write (s_mem, buffer, (int)length) <= 0 ||
            BIO_flush (s_mem) <= 0) {
	        BIO_free_all (s_mem);
                error_code (SystemCall, 3);
                return NULL; 
        }
	BIO_get_mem_ptr (s_mem, &pointer);
	BIO_set_close (s_mem, BIO_NOCLOSE);
        if (!(encoded = string_create_with_size (pointer->length + 1))) {
	        BIO_free_all (s_mem);
                error (FunctionCall);
                return NULL;
        }
        (void)memmove (encoded, pointer->data, pointer->length);
        encoded[pointer->length] = '\0';
	BIO_free_all (s_mem);
	return encoded;
}
