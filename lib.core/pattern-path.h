#ifndef pattern_path_h
#define pattern_path_h

#include <lib.core/defines.h>
#include <lib.core/list.h>
#include <lib.core/pattern-branch.h>

typedef struct s_PatternPath         PatternPath;
typedef struct s_PatternPathBranches PatternPathBranches;
typedef struct s_PatternPathRepeat   PatternPathRepeat;
typedef struct s_PatternPathRange    PatternPathRange;
typedef struct s_PatternPathSet      PatternPathSet;
typedef struct s_PatternPathValue    PatternPathValue;

struct s_PatternPath
{
        enum {
                PatternPathTypeBranches,
                PatternPathTypeRepeat,
                PatternPathTypeRange,
                PatternPathTypeSet,
                PatternPathTypeValue
        } type;
};

struct s_PatternPathBranches
{
        PatternPath base;
        PatternPath **array;
        size_t array_length;
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
        PatternTokenSet token;
        PatternPath *next;
        bool not;
};

PatternPath *pattern_path_create (PatternBranch *branch);

#endif
