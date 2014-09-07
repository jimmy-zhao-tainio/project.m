#include <lib.core/memory.h>
#include <lib.core/error.h>
#include <lib.core/list.h>
#include <lib.pattern/pattern-path.h>
#include <stdio.h>

static PatternPath *path_create_branch (PatternBranch *branch, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part (PatternBranchPart *part, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_branch (PatternBranchPartBranch *branch, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_not (PatternBranchPartNot *not, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_or (PatternBranchPartOr *or, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_repeat (PatternBranchPartRepeat *repeat, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_range (PatternBranchPartRange *range, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_set (PatternBranchPartSet *set, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_value (PatternBranchPartValue *value, PatternPath *last, PatternPathTracker *tracker);
static void paths_destroy (List *paths);

PatternPathTracker *pattern_path_tracker_create (PatternBranch *branch)
{
        PatternPathTracker *tracker;

        if (!branch) {
                error (InvalidArgument);
                return NULL;
        }
        if (!(tracker = memory_create (sizeof (PatternPathTracker)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(tracker->paths = list_create ())) {
                memory_destroy (tracker);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(tracker->path = path_create_branch (branch, NULL, tracker))) {
                paths_destroy (tracker->paths);
                memory_destroy (tracker);
                error_code (FunctionCall, 3);
                return NULL;
        }
        return tracker;
}

void pattern_path_tracker_destroy (PatternPathTracker *tracker)
{
        if (!tracker) {
                return;
        }
        paths_destroy (tracker->paths);
        memory_destroy (tracker);
}

static PatternPath *path_create_branch (PatternBranch *branch, PatternPath *last, PatternPathTracker *tracker)
{
        PatternPath *path = NULL;
        ListNode *node;

        for (node = list_last (branch->parts); node; node = node->previous) {
                if (!(path = path_create_part (node->data, last, tracker))) {
                        error (FunctionCall);
                        return NULL;
                }
                last = path;
        }
        return path;
}

static PatternPath *path_create_part (PatternBranchPart *part, PatternPath *last, PatternPathTracker *tracker)
{
        switch (part->type) {
        case PatternBranchPartTypeBranch:
                return path_create_part_branch ((PatternBranchPartBranch *)part, last, tracker);
        case PatternBranchPartTypeNot:
                return path_create_part_not ((PatternBranchPartNot *)part, last, tracker);
        case PatternBranchPartTypeOr:
                return path_create_part_or ((PatternBranchPartOr *)part, last, tracker);
        case PatternBranchPartTypeRepeat:
                return path_create_part_repeat ((PatternBranchPartRepeat *)part, last, tracker);
        case PatternBranchPartTypeRange:
                return path_create_part_range ((PatternBranchPartRange *)part, last, tracker);
        case PatternBranchPartTypeSet:
                return path_create_part_set ((PatternBranchPartSet *)part, last, tracker);
        case PatternBranchPartTypeValue:
                return path_create_part_value ((PatternBranchPartValue *)part, last, tracker);
        default:
                return NULL;
        };
}

static PatternPath *path_create_part_branch (PatternBranchPartBranch *branch, PatternPath *last, PatternPathTracker *tracker)
{
        return path_create_branch (branch->branch, last, tracker);
}

static PatternPath *path_create_part_not (PatternBranchPartNot *not, PatternPath *last, PatternPathTracker *tracker)
{
        PatternPath *path;

        if (!(path = path_create_part (not->part, last, tracker))) {
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
                error (InvalidOperation);
                return NULL;
        };
        return path;
}

static PatternPath *path_create_part_or (PatternBranchPartOr *or, PatternPath *last, PatternPathTracker *tracker)
{
        PatternPathOr *path;

        if (!(path = memory_create (sizeof (PatternPathOr)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(list_append (tracker->paths, path))) {
                memory_destroy (path);
                error_code (FunctionCall, 2);
                return NULL;
        }
        path->base.type = PatternPathTypeOr;
        if (!(path->left = path_create_branch (or->left, last, tracker))) {
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (!(path->right = path_create_branch (or->right, last, tracker))) {
                error_code (FunctionCall, 4);
                return NULL;
        }
        return (PatternPath *)path;
}

static PatternPath *path_create_part_repeat (PatternBranchPartRepeat *repeat, PatternPath *last, PatternPathTracker *tracker)
{
        PatternPathRepeat *path;

        if (!(path = memory_create (sizeof (PatternPathRepeat)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(list_append (tracker->paths, path))) {
                memory_destroy (path);
                error_code (FunctionCall, 2);
                return NULL;
        }
        path->next_repeat = tracker->repeat;
        tracker->repeat = path;
        path->base.type = PatternPathTypeRepeat;
        if (!(path->repeat = path_create_part (repeat->part, (PatternPath *)path, tracker))) {
                error_code (FunctionCall, 3);
                return NULL;
        }
        path->token.from = repeat->token->from;
        path->token.to = repeat->token->to;
        path->next = last;
        path->repeated = 0;
        return (PatternPath *)path;
}

static PatternPath *path_create_part_range (PatternBranchPartRange *range, PatternPath *last, PatternPathTracker *tracker)
{
        PatternPathRange *path;

        if (!(path = memory_create (sizeof (PatternPathRange)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(list_append (tracker->paths, path))) {
                memory_destroy (path);
                error_code (FunctionCall, 2);
                return NULL;
        }
        path->base.type = PatternPathTypeRange;
        path->token.from = range->token->from;
        path->token.to = range->token->to;
        path->next = last;
        return (PatternPath *)path;
}

static PatternPath *path_create_part_set (PatternBranchPartSet *set, PatternPath *last, PatternPathTracker *tracker)
{
        PatternPathSet *path;

        if (!(path = memory_create (sizeof (PatternPathSet)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(list_append (tracker->paths, path))) {
                memory_destroy (path);
                error_code (FunctionCall, 2);
                return NULL;
        }
        path->base.type = PatternPathTypeSet;
        path->token.values = set->token->values;
        path->token.values_length = set->token->values_length;
        path->next = last;
        return (PatternPath *)path;
}

static PatternPath *path_create_part_value (PatternBranchPartValue *value, PatternPath *last, PatternPathTracker *tracker)
{
        PatternPathValue *path;

        if (!(path = memory_create (sizeof (PatternPathValue)))) {
                error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(list_append (tracker->paths, path))) {
                memory_destroy (path);
                error_code (FunctionCall, 2);
                return NULL;
        }
        path->base.type = PatternPathTypeValue;
        path->token.value = value->token->value;
        path->next = last;
        return (PatternPath *)path;
}

static void paths_destroy (List *paths)
{
        ListNode *node;

        if (!paths) {
                return;
        }
        while (list_count (paths) != 0) {
                node = list_first (paths);
                memory_destroy (node->data);
                list_remove (paths, node);
        }
        list_destroy (paths);
}
