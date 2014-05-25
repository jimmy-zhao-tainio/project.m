#ifndef pattern_path_h
#define pattern_path_h

#include <lib.core/defines.h>
#include <lib.core/list.h>
#include <lib.core/pattern-branch.h>

typedef struct s_PatternPath       PatternPath;
typedef struct s_PatternPathOr     PatternPathOr;
typedef struct s_PatternPathRepeat PatternPathRepeat;
typedef struct s_PatternPathRange  PatternPathRange;
typedef struct s_PatternPathSet    PatternPathSet;
typedef struct s_PatternPathValue  PatternPathValue;

struct s_PatternPath
{
        enum {
                PatternPathTypeOr,
                PatternPathTypeRepeat,
                PatternPathTypeRange,
                PatternPathTypeSet,
                PatternPathTypeValue
        } type;

        bool destroy;
};

struct s_PatternPathOr
{
        PatternPath base;
        PatternPath *left;
        PatternPath *right;
};

struct s_PatternPathRepeat
{
        PatternPath base;
        PatternTokenRepeat token;
        PatternPath *repeat;
        PatternPath *next;
};

struct s_PatternPathRange
{
        PatternPath base;
        PatternTokenRange token;
        PatternPath *next;
        bool not;
};

struct s_PatternPathSet
{
        PatternPath base;
        PatternTokenSet token;
        PatternPath *next;
        bool not;
};

struct s_PatternPathValue
{
        PatternPath base;
        PatternTokenValue token;
        PatternPath *next;
        bool not;
};

PatternPath *pattern_path_create (PatternBranch *branch);
void pattern_path_destroy (PatternPath *path);

#endif
