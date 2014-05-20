#ifndef pattern_branch_h
#define pattern_branch_h

#include <lib.core/list.h>
#include <lib.core/pattern-token.h>

#define PATTERN_BRANCH_RECURSION_DEPTH 255

typedef struct s_PatternBranch           PatternBranch;
typedef struct s_PatternBranchPart       PatternBranchPart;
typedef struct s_PatternBranchPartBranch PatternBranchPartBranch;
typedef struct s_PatternBranchPartNot    PatternBranchPartNot;
typedef struct s_PatternBranchPartOr     PatternBranchPartOr;
typedef struct s_PatternBranchPartRepeat PatternBranchPartRepeat;
typedef struct s_PatternBranchPartRange  PatternBranchPartRange;
typedef struct s_PatternBranchPartSet    PatternBranchPartSet;
typedef struct s_PatternBranchPartValue  PatternBranchPartValue;

struct s_PatternBranch
{
	List *parts;
};

struct s_PatternBranchPart
{
	enum {
		PatternBranchPartTypeBranch,
		PatternBranchPartTypeNot,
		PatternBranchPartTypeOr,
		PatternBranchPartTypeRepeat,
		PatternBranchPartTypeRange,
		PatternBranchPartTypeSet,
		PatternBranchPartTypeValue,
	} type;
};

struct s_PatternBranchPartBranch
{
	PatternBranchPart base;
	PatternBranch *branch;
};

struct s_PatternBranchPartNot
{
	PatternBranchPart base;
	PatternBranchPart *part;
};

struct s_PatternBranchPartOr
{
	PatternBranchPart base;
	PatternBranch *left;
	PatternBranch *right;
};

struct s_PatternBranchPartRepeat
{
	PatternBranchPart base;
	PatternBranchPart *part;
	PatternTokenRepeat *token;
};

struct s_PatternBranchPartRange
{
	PatternBranchPart base;
	PatternTokenRange *token;
};

struct s_PatternBranchPartSet
{
	PatternBranchPart base;
	PatternTokenSet *token;
};

struct s_PatternBranchPartValue
{
	PatternBranchPart base;
	PatternTokenValue *token;
};

PatternBranch *pattern_branch_create (List *tokens);
void pattern_branch_destroy (PatternBranch *branch);

#endif
