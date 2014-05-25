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
static void pattern_path_destroy_or (PatternPathOr *or);
static void pattern_path_destroy_repeat (PatternPathRepeat *repeat);
static void pattern_path_destroy_range (PatternPathRange *range);
static void pattern_path_destroy_set (PatternPathSet *set);
static void pattern_path_destroy_value (PatternPathValue *value);

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
        };
}

static PatternPath *path_create_part_branch (PatternBranchPartBranch *branch, PatternPath *last)
{
        return path_create_branch (branch->branch, last);
}

static PatternPath *path_create_part_not (PatternBranchPartNot *not, PatternPath *last)
{
        PatternPath *path;

        if (!(path = path_create_part (not->part, last))) {
                error (FunctionCall);
                return NULL;
        }
        switch (not->part->type) {
        case PatternBranchPartTypeRange:
                ((PatternPathRange *)path)->not = true;
                break;
        case PatternBranchPartTypeSet:
                ((PatternPathSet *)path)->not = true;
                break;
        case PatternBranchPartTypeValue:
                ((PatternPathValue *)path)->not = true;
                break;
        default:
                memory_destroy (path);
                error (InvalidOperation);
                return NULL;
        };
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
        path->base.destroy = true;
        if (!(path->left = path_create_branch (or->left, last))) {
                memory_destroy (path);
                error (FunctionCall);
                return NULL;
        }
        if (!(path->right = path_create_branch (or->right, last))) {
                pattern_path_destroy (path->left);
                memory_destroy (path);
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
        path->base.destroy = false;
        if (!(path->repeat = path_create_part (repeat->part, (PatternPath *)path))) {
                memory_destroy (path);
                error (FunctionCall);
                return NULL;
        }
        path->base.destroy = true;
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
        path->base.destroy = true;
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
        path->base.destroy = true;
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
        path->base.destroy = true;
        path->token.value = value->token->value;
        path->next = last;
        return (PatternPath *)path;
}

void pattern_path_destroy (PatternPath *path)
{
        if (!path) {
                return;
        }
        if (!path->destroy) {
                return;
        }
        path->destroy = false;
        switch (path->type) {
        case PatternPathTypeOr:
                pattern_path_destroy_or ((PatternPathOr *)path);
                break;
        case PatternPathTypeRepeat:
                pattern_path_destroy_repeat ((PatternPathRepeat *)path);
                break;
        case PatternPathTypeRange:
                pattern_path_destroy_range ((PatternPathRange *)path);
                break;
        case PatternPathTypeSet:
                pattern_path_destroy_set ((PatternPathSet *)path);
                break;
        case PatternPathTypeValue:
                pattern_path_destroy_value ((PatternPathValue *)path);
                break;
        };
}

static void pattern_path_destroy_or (PatternPathOr *or)
{
        if (!or) {
                return;
        }
        pattern_path_destroy (or->left);
        pattern_path_destroy (or->right);
        memory_destroy (or);
}

static void pattern_path_destroy_repeat (PatternPathRepeat *repeat)
{
        if (!repeat) {
                return;
        }
        pattern_path_destroy (repeat->next);
        pattern_path_destroy (repeat->repeat);
        memory_destroy (repeat);
}

static void pattern_path_destroy_range (PatternPathRange *range)
{
        if (!range) {
                return;
        }
        pattern_path_destroy (range->next);
        memory_destroy (range);
}

static void pattern_path_destroy_set (PatternPathSet *set)
{
        if (!set) {
                return;
        }
        pattern_path_destroy (set->next);
        memory_destroy (set);
}

static void pattern_path_destroy_value (PatternPathValue *value)
{
        if (!value) {
                return;
        }
        pattern_path_destroy (value->next);
        memory_destroy (value);
}
