#ifndef pattern_token_h
#define pattern_token_h

#include <lib.core/defines.h>
#include <lib.core/list.h>

typedef enum {
	PatternTokenTypeParenthesesOpen,
	PatternTokenTypeParenthesesClose,
	PatternTokenTypeNot,
	PatternTokenTypeOr,
	PatternTokenTypeRepeat,
	PatternTokenTypeRange,
	PatternTokenTypeSet,
	PatternTokenTypeValue,
} PatternTokenType;

typedef struct s_PatternToken       PatternToken;
typedef struct s_PatternTokenRepeat PatternTokenRepeat;
typedef struct s_PatternTokenRange  PatternTokenRange;
typedef struct s_PatternTokenSet    PatternTokenSet;
typedef struct s_PatternTokenValue  PatternTokenValue;

struct s_PatternToken
{
	PatternTokenType type;
};

struct s_PatternTokenRepeat
{
	PatternToken token;
	size_t from;
	size_t to;
};

struct s_PatternTokenRange
{
	PatternToken token;
	unsigned char from;
	unsigned char to;
};

struct s_PatternTokenSet
{
	PatternToken token;
	unsigned char *values;
	size_t values_length;
};

struct s_PatternTokenValue
{
	PatternToken token;
	unsigned char value;
};

List *pattern_tokens_create (const char *pattern);
void pattern_tokens_destroy (List *tokens); 

#endif
