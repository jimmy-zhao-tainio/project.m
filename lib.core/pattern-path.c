#include <lib.core/pattern-path.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

static PatternPath *path_create_branch (PatternBranch *branch, PatternPath *last);
static PatternPath *path_create_part (PatternBranchPart *part, PatternPath *last);
static PatternPath *path_create_part_branch (PatternBranchPartBranch *branch, PatternPath *last);
static PatternPath *path_create_part_not (PatternBranchPartNot *not, PatternPath *last);
static PatternPath *path_create_part_or (PatternBranchPartOr *or, PatternPath *last);
static PatternPath *path_create_part_repeat (PatternBranchPartRepeat *repeat, PatternPath *last);
static PatternPath *path_create_part_range (PatternBranchPartRange *range, PatternPath *last);
static PatternPath *path_create_part_set (PatternBranchPartSet *set, PatternPath *last);
static PatternPath *path_create_part_value (PatternBranchPartValue *value, PatternPath *last);

PatternPath *pattern_path_create (PatternBranch *branch)
{
        PatternPath *path = NULL;

        if (!branch) {
                error (InvalidArgument);
                return NULL;
        }
        if (!(path = path_create_branch (branch, NULL))) {
                error (FunctionCall);
                return NULL;
        }
        return path;
}

void pattern_path_destroy (PatternPath *path)
{
        if (!path) {
                error (InvalidArgument);
                return;
        }
        memory_destroy (path);
}

static PatternPath *path_create_branch (PatternBranch *branch, PatternPath *last)
{
        PatternPath *path;
        ListNode *node;

        for (node = list_last (branch->parts); node; node = node->previous) {
                if (!(path = path_create_part (node->data, last))) {
                        if (last) {
                                pattern_path_destroy (last);
                        }
                        error (FunctionCall);
                        return NULL;
                }
                last = path;
        }
        return path;
}

static PatternPath *path_create_part (PatternBranchPart *part, PatternPath *last)
{
        switch (part->type) {
        case PatternBranchPartTypeBranch:
                return path_create_part_branch ((PatternBranchPartBranch *)part, last);
        case PatternBranchPartTypeNot:
                return path_create_part_not ((PatternBranchPartNot *)part, last);
        case PatternBranchPartTypeOr:
                return path_create_part_or ((PatternBranchPartOr *)part, last);
        case PatternBranchPartTypeRepeat:
                return path_create_part_repeat ((PatternBranchPartRepeat *)part, last);
        case PatternBranchPartTypeRange:
                return path_create_part_range ((PatternBranchPartRange *)part, last);
        case PatternBranchPartTypeSet:
                return path_create_part_set ((PatternBranchPartSet *)part, last);
        case PatternBranchPartTypeValue:
                return path_create_part_value ((PatternBranchPartValue *)part, last);
        default:
                return NULL;
        };
}

static PatternPath *path_create_part_branch (PatternBranchPartBranch *branch, PatternPath *last)
{
        return path_create_branch (branch->branch, last);
}

static PatternPath *path_create_part_not (PatternBranchPartNot *not, PatternPath *last)
{
        PatternPath *path;

        if (not->part->type != PatternBranchPartTypeRange &&
            not->part->type != PatternBranchPartTypeSet &&
            not->part->type != PatternBranchPartTypeValue) {
                error (InvalidOperation);
                return NULL;
        }
        if (!(path = path_create_part (not->part, last))) {
                error (FunctionCall);
                return NULL;
        }
        if (not->part->type == PatternBranchPartTypeRange) {
                ((PatternPathRange *)path)->not = true;
        }
        else if (not->part->type == PatternBranchPartTypeSet) {
                ((PatternPathSet *)path)->not = true;
        }
        else {
                ((PatternPathValue *)path)->not = true;
        }
        return path;
}

static PatternPath *path_create_part_or (PatternBranchPartOr *or, PatternPath *last)
{
        PatternPathOr *path;

        if (!(path = memory_create (sizeof (PatternPathOr)))) {
                error (FunctionCall);
                return NULL;
        }
        path->base.type = PatternPathTypeOr;
        if (!(path->left = path_create_branch (or->left, last))) {
                error (FunctionCall);
                return NULL;
        }
        if (!(path->right = path_create_branch (or->right, last))) {
                error (FunctionCall);
                return NULL;
        }
        return (PatternPath *)path;
}

static PatternPath *path_create_part_repeat (PatternBranchPartRepeat *repeat, PatternPath *last)
{
        PatternPathRepeat *path;

        if (!(path = memory_create (sizeof (PatternPathRepeat)))) {
                error (FunctionCall);
                return NULL;
        }
        path->base.type = PatternPathTypeRepeat;
        if (!(path->repeat = path_create_part (repeat->part, (PatternPath *)path))) {
                error (FunctionCall);
                return NULL;
        }
        path->token.from = repeat->token->from;
        path->token.to = repeat->token->to;
        path->next = last;
        return (PatternPath *)path;
}

static PatternPath *path_create_part_range (PatternBranchPartRange *range, PatternPath *last)
{
        PatternPathRange *path;

        if (!(path = memory_create (sizeof (PatternPathRange)))) {
                error (FunctionCall);
                return NULL;
        }
        path->base.type = PatternPathTypeRange;
        path->token.from = range->token->from;
        path->token.to = range->token->to;
        path->next = last;
        return (PatternPath *)path;
}

static PatternPath *path_create_part_set (PatternBranchPartSet *set, PatternPath *last)
{
        PatternPathSet *path;

        if (!(path = memory_create (sizeof (PatternPathSet)))) {
                error (FunctionCall);
                return NULL;
        }
        path->base.type = PatternPathTypeSet;
        path->token.values = set->token->values;
        path->token.values_length = set->token->values_length;
        path->next = last;
        return (PatternPath *)path;
}

static PatternPath *path_create_part_value (PatternBranchPartValue *value, PatternPath *last)
{
        PatternPathValue *path;

        if (!(path = memory_create (sizeof (PatternPathValue)))) {
                error (FunctionCall);
                return NULL;
        }
        path->base.type = PatternPathTypeValue;
        path->token.value = value->token->value;
        path->next = last;
        return (PatternPath *)path;
}
