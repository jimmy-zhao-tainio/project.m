#include <lib.core/pattern-path.h>
#include <lib.core/memory.h>
#include <lib.core/error.h>

static PatternPath *path_create_branch (PatternBranch *branch, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part (PatternBranchPart *part, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_branch (PatternBranchPartBranch *branch, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_not (PatternBranchPartNot *not, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_or (PatternBranchPartOr *or, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_repeat (PatternBranchPartRepeat *repeat, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_range (PatternBranchPartRange *range, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_set (PatternBranchPartSet *set, PatternPath *last, PatternPathTracker *tracker);
static PatternPath *path_create_part_value (PatternBranchPartValue *value, PatternPath *last, PatternPathTracker *tracker);
static void path_destroy (PatternPath *path);
static void path_destroy_or (PatternPathOr *or);
static void path_destroy_repeat (PatternPathRepeat *repeat);
static void path_destroy_range (PatternPathRange *range);
static void path_destroy_set (PatternPathSet *set);
static void path_destroy_value (PatternPathValue *value);
static bool path_tracker_add_repeat (PatternPathTracker *tracker, PatternPathRepeat *repeat);
static bool path_tracker_add_follow (PatternPathTracker *tracker);

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
        tracker->follow_index = 0;
        tracker->repeat_index = 0;
        tracker->follow_length = 64;
        tracker->repeat_length = 8;
        if (!(tracker->follow = memory_create (tracker->follow_length * sizeof (PatternPath *)))) {
                memory_destroy (tracker);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(tracker->repeat = memory_create (tracker->repeat_length * sizeof (PatternPathRepeat *)))) {
                memory_destroy (tracker->follow);
                memory_destroy (tracker);
                error_code (FunctionCall, 3);
                return NULL;
        }
        if (!(tracker->path = path_create_branch (branch, NULL, tracker))) {
                memory_destroy (tracker->follow);
                memory_destroy (tracker->repeat);
                memory_destroy (tracker);
                error_code (FunctionCall, 4);
                return NULL;
        }
        return tracker;
}

void pattern_path_tracker_destroy (PatternPathTracker *tracker)
{
        if (!tracker) {
                return;
        }
        if (tracker->follow) {
                memory_destroy (tracker->follow);
        }
        if (tracker->repeat) {
                memory_destroy (tracker->repeat);
        }
        if (tracker->path) {
                path_destroy (tracker->path);
        }
        memory_destroy (tracker);
}

static PatternPath *path_create_branch (PatternBranch *branch, PatternPath *last, PatternPathTracker *tracker)
{
        PatternPath *path;
        ListNode *node;

        for (node = list_last (branch->parts); node; node = node->previous) {
                if (!(path = path_create_part (node->data, last, tracker))) {
                        if (last) {
                                path_destroy (last);
                                error_code (FunctionCall, 1);
                        }
                        else {
                                error_code (FunctionCall, 2);
                        }
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
                memory_destroy (path);
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
        path->base.type = PatternPathTypeOr;
        path->base.destroy = true;
        if (!(path->left = path_create_branch (or->left, last, tracker))) {
                memory_destroy (path);
                error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(path->right = path_create_branch (or->right, last, tracker))) {
                path_destroy (path->left);
                memory_destroy (path);
                error_code (FunctionCall, 3);
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
        if (!path_tracker_add_repeat (tracker, path)) {
                memory_destroy (path);
                error_code (FunctionCall, 2);
                return NULL;
        }
        path->base.type = PatternPathTypeRepeat;
        path->base.destroy = false;
        if (!(path->repeat = path_create_part (repeat->part, (PatternPath *)path, tracker))) {
                memory_destroy (path);
                error_code (FunctionCall, 3);
                return NULL;
        }
        path->base.destroy = true;
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
        if (!path_tracker_add_follow (tracker)) {
                memory_destroy (path);
                error_code (FunctionCall, 2);
                return NULL;
        }
        path->base.type = PatternPathTypeRange;
        path->base.destroy = true;
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
        if (!path_tracker_add_follow (tracker)) {
                memory_destroy (path);
                error_code (FunctionCall, 2);
                return NULL;
        }
        path->base.type = PatternPathTypeSet;
        path->base.destroy = true;
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
        if (!path_tracker_add_follow (tracker)) {
                memory_destroy (path);
                error_code (FunctionCall, 2);
                return NULL;
        }
        path->base.type = PatternPathTypeValue;
        path->base.destroy = true;
        path->token.value = value->token->value;
        path->next = last;
        return (PatternPath *)path;
}

static void path_destroy (PatternPath *path)
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
                path_destroy_or ((PatternPathOr *)path);
                break;
        case PatternPathTypeRepeat:
                path_destroy_repeat ((PatternPathRepeat *)path);
                break;
        case PatternPathTypeRange:
                path_destroy_range ((PatternPathRange *)path);
                break;
        case PatternPathTypeSet:
                path_destroy_set ((PatternPathSet *)path);
                break;
        case PatternPathTypeValue:
                path_destroy_value ((PatternPathValue *)path);
                break;
        };
}

static void path_destroy_or (PatternPathOr *or)
{
        if (!or) {
                return;
        }
        path_destroy (or->left);
        path_destroy (or->right);
        memory_destroy (or);
}

static void path_destroy_repeat (PatternPathRepeat *repeat)
{
        if (!repeat) {
                return;
        }
        path_destroy (repeat->next);
        path_destroy (repeat->repeat);
        memory_destroy (repeat);
}

static void path_destroy_range (PatternPathRange *range)
{
        if (!range) {
                return;
        }
        path_destroy (range->next);
        memory_destroy (range);
}

static void path_destroy_set (PatternPathSet *set)
{
        if (!set) {
                return;
        }
        path_destroy (set->next);
        memory_destroy (set);
}

static void path_destroy_value (PatternPathValue *value)
{
        if (!value) {
                return;
        }
        path_destroy (value->next);
        memory_destroy (value);
}

static bool path_tracker_add_repeat (PatternPathTracker *tracker, PatternPathRepeat *repeat)
{
        PatternPath **grow;

        if (tracker->repeat_index == tracker->repeat_length) {
                if (!(grow = memory_grow (tracker->repeat, 2 * (tracker->repeat_length * sizeof (PatternPathRepeat *))))) {
                        error (FunctionCall);
                        return false;
                }
                tracker->repeat_length *= 2;
                tracker->repeat = (PatternPathRepeat **)grow;
        }
        tracker->repeat[tracker->repeat_index] = repeat;
        tracker->repeat_index++;
        return true;
}

static bool path_tracker_add_follow (PatternPathTracker *tracker)
{
        PatternPath **grow;

        if (tracker->follow_index == tracker->follow_length) {
                if (!(grow = memory_grow (tracker->follow, 2 * (tracker->follow_length * sizeof (PatternPath *))))) {
                        error (FunctionCall);
                        return false;
                }
                tracker->follow_length *= 2;
        }
        tracker->follow_index++;
        return true;
}
