#ifndef pattern_path_h
#define pattern_path_h

#include <lib.core/defines.h>
#include <lib.core/list.h>
#include <lib.core/pattern-branch.h>

typedef struct s_PatternPathTracker PatternPathTracker;
typedef struct s_PatternPath        PatternPath;
typedef struct s_PatternPathOr      PatternPathOr;
typedef struct s_PatternPathRepeat  PatternPathRepeat;
typedef struct s_PatternPathRange   PatternPathRange;
typedef struct s_PatternPathSet     PatternPathSet;
typedef struct s_PatternPathValue   PatternPathValue;

struct s_PatternPathTracker
{
        PatternPath **follow;
        size_t follow_length;
        size_t follow_index;
        PatternPathRepeat **repeat;
        size_t repeat_length;
        size_t repeat_index;
        PatternPath *path;
};

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

PatternPathTracker *pattern_path_tracker_create (PatternBranch *branch);
void pattern_path_tracker_destroy (PatternPathTracker *tracker);

#endif
