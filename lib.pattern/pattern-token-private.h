#ifndef pattern_token_private_h
#define pattern_token_private_h

bool pattern_token_string_to_value (const char *pattern, size_t length, size_t *i, unsigned char *value);
bool pattern_token_escape_to_value (const char *pattern, size_t length, size_t *i, unsigned char *value);
bool pattern_token_escapable_to_byte (char escapable, unsigned char *value);
bool pattern_token_is_syntax (char c);
bool pattern_token_is_octal (char c1, char c2, char c3);
bool pattern_token_is_hexadecimal (char c1, char c2);
bool pattern_token_is_binary (char c1, char c2, char c3, char c4, char c5, char c6, char c7, char c8);

#endif
