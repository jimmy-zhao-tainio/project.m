#ifndef pattern_path_h
#define pattern_path_h

#include <lib.core/defines.h>
#include <lib.core/list.h>
#include <lib.pattern/pattern-branch.h>

typedef struct s_PatternPathTracker PatternPathTracker;
typedef struct s_PatternPath        PatternPath;
typedef struct s_PatternPathOr      PatternPathOr;
typedef struct s_PatternPathRepeat  PatternPathRepeat;
typedef struct s_PatternPathRange   PatternPathRange;
typedef struct s_PatternPathSet     PatternPathSet;
typedef struct s_PatternPathValue   PatternPathValue;

struct s_PatternPathTracker
{
        PatternPathRepeat *repeat;
        PatternPath *path;
        PatternPath *track_first;
        PatternPath *track_last;
        size_t track_count;
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

        PatternPath *track_previous;
        PatternPath *track_next;
        bool         track_pushed;
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
        PatternPathRepeat *next_repeat;
        unsigned long long repeated;
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
