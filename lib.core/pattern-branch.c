#include <lib.core/defines.h>
#include <lib.core/memory.h>
#include <lib.core/list.h>
#include <lib.core/string.h>
#include <lib.core/error.h>
#include <lib.core/pattern-token.h>
#include <lib.core/pattern-branch.h>

static bool parenthesize_main_branch (List *tokens);
static PatternBranch *branch_create (ListNode *node, ListNode **end, uint8_t depth);
static PatternBranchPart *branch_part_create (ListNode *node, ListNode **end, uint8_t depth);
static PatternBranchPart *branch_part_create_branch (ListNode *node, ListNode **end, uint8_t depth);
static PatternBranchPart *branch_part_create_not (ListNode *node, ListNode **end, uint8_t depth);
static PatternBranchPart *branch_part_create_or (ListNode *node, ListNode **end);
static PatternBranchPart *branch_part_create_repeat (ListNode *node, ListNode **end);
static PatternBranchPart *branch_part_create_range (ListNode *node, ListNode **end);
static PatternBranchPart *branch_part_create_set (ListNode *node, ListNode **end);
static PatternBranchPart *branch_part_create_value (ListNode *node, ListNode **end);
static void branch_destroy (PatternBranch *branch);
static void branch_part_destroy (PatternBranchPart *part);
static void branch_part_destroy_branch (PatternBranchPartBranch *branch);
static void branch_part_destroy_not (PatternBranchPartNot *not);
static void branch_part_destroy_or (PatternBranchPartOr *or);
static void branch_part_destroy_repeat (PatternBranchPartRepeat *repeat);
static void branch_part_destroy_range (PatternBranchPartRange *range);
static void branch_part_destroy_set (PatternBranchPartSet *set);
static void branch_part_destroy_value (PatternBranchPartValue *value);

#define pattern_error(e) if (error_at (0).error != ErrorPatternBranchRecursionDepth) error(e);
#define pattern_error_code(e,c) if (error_at (0).error != ErrorPatternBranchRecursionDepth) error_code(e,c);

PatternBranch *pattern_branch_create (List *tokens)
{
        PatternBranch *branch;
        ListNode *end = NULL;
        uint8_t depth = PATTERN_BRANCH_RECURSION_DEPTH;

        if (!tokens) {
                pattern_error (InvalidArgument);
                return NULL;
        }
        if (!parenthesize_main_branch (tokens)) {
                pattern_error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(branch = branch_create (list_first (tokens), &end, depth))) {
                pattern_error_code (FunctionCall, 2);
                return NULL;
        }
        if (end) {
                pattern_error (InvalidOperation);
                pattern_branch_destroy (branch);
                return NULL;
        }
        return branch;
}

void pattern_branch_destroy (PatternBranch *branch)
{
        if (!branch) {
                pattern_error (InvalidArgument);
                return;
        }
        branch_destroy (branch);
}

static bool parenthesize_main_branch (List *tokens)
{
        PatternToken *token;

        if (!(token = memory_create (sizeof (PatternToken)))) {
                pattern_error_code (FunctionCall, 1);
                return false;
        }
        token->type = PatternTokenTypeParenthesesOpen;
        if (!list_insert (tokens, token)) {
                memory_destroy (token);
                pattern_error_code (FunctionCall, 2);
                return false;
        }
        if (!(token = memory_create (sizeof (PatternToken)))) {
                pattern_error_code (FunctionCall, 3);
                return false;
        }
        token->type = PatternTokenTypeParenthesesClose;
        if (!list_append (tokens, token)) {
                memory_destroy (token);
                pattern_error_code (FunctionCall, 4);
                return false;
        }
        return true;
}

static PatternBranch *branch_create (ListNode *node, ListNode **end, uint8_t depth)
{
        PatternBranch *branch;
        PatternBranchPart *part;
        PatternBranchPartOr *or = NULL;
        PatternToken *token;
        List *list;
        ListNode *node_last;

        token = node->data;
        if (!(branch = memory_create (sizeof (PatternBranch)))) {
                pattern_error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(branch->parts = list_create ())) {
                memory_destroy (branch);
                pattern_error_code (FunctionCall, 2);
                return NULL;
        }
        node = node->next;
        while (node) {
                token = node->data;
                if (token->type == PatternTokenTypeParenthesesClose) {
                        *end = node->next;
                        return branch;
                }
                if (!(part = branch_part_create (node, end, depth))) {
                        pattern_branch_destroy (branch);
                        pattern_error_code (FunctionCall, 3);
                        return NULL;
                }
                if (part->type == PatternBranchPartTypeOr) {
                        or = (PatternBranchPartOr *)part;
                        list = or->left->parts;
                        or->left->parts = branch->parts;
                        branch->parts = list;
                        if (!list_insert (branch->parts, or)) {
                                branch_part_destroy (part);
                                pattern_branch_destroy (branch);
                                pattern_error_code (FunctionCall, 4);
                                return NULL;
                        }
                        node = *end;
                        continue;
                }
                list = or ? or->right->parts : branch->parts;
                node_last = list_last (list);
                if (part->type == PatternBranchPartTypeRepeat) {
                        ((PatternBranchPartRepeat *)part)->part = node_last->data;
                        node_last->data = part;
                        node = *end;
                        continue;
                }
                if (!list_append (list, part)) {
                        branch_part_destroy (part);
                        pattern_branch_destroy (branch);
                        pattern_error_code (FunctionCall, 5);
                        return NULL;
                }
                node = *end;
        }
        pattern_branch_destroy (branch);
        pattern_error (InvalidOperation);
        return NULL;
}

static PatternBranchPart *branch_part_create (ListNode *node, ListNode **end, uint8_t depth)
{
        PatternToken *token = node->data;

        switch (token->type) {
        case PatternTokenTypeParenthesesOpen:
                return branch_part_create_branch (node, end, depth);
        case PatternTokenTypeNot:
                return branch_part_create_not (node, end, depth);
        case PatternTokenTypeOr:
                return branch_part_create_or (node, end);
        case PatternTokenTypeRepeat:
                return branch_part_create_repeat (node, end);
        case PatternTokenTypeRange:
                return branch_part_create_range (node, end);
        case PatternTokenTypeSet:
                return branch_part_create_set (node, end);
        default:        
                return branch_part_create_value (node, end);
        }
}

static PatternBranchPart *branch_part_create_branch (ListNode *node, ListNode **end, uint8_t depth)
{
        PatternBranchPartBranch *branch;
        
        if (depth == 0) {
                pattern_error_code (PatternBranchRecursionDepth, 2);
                return NULL;
        }
        if (!(branch = memory_create (sizeof (PatternBranchPartBranch)))) {
                pattern_error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(branch->branch = branch_create (node, end, (uint8_t)(depth - 1)))) {
                branch_part_destroy_branch (branch);
                pattern_error_code (FunctionCall, 2);
                return NULL;
        }
        branch->base.type = PatternBranchPartTypeBranch;
        return (PatternBranchPart *)branch;
}

static PatternBranchPart *branch_part_create_not (ListNode *node, ListNode **end, uint8_t depth)
{
        PatternBranchPartNot *not;

        if (!(not = memory_create (sizeof (PatternBranchPartNot)))) {
                pattern_error_code (FunctionCall, 1);
                return NULL;
        }
        not->base.type = PatternBranchPartTypeNot;
        if (!(not->part = branch_part_create (node->next, end, (uint8_t)(depth - 1)))) {
                branch_part_destroy_not (not);
                pattern_error_code (FunctionCall, 2);
                return NULL;
        }
        return (PatternBranchPart *)not;
}

static PatternBranchPart *branch_part_create_or (ListNode *node, ListNode **end)
{
        PatternBranchPartOr *or;
        
        if (!(or = memory_create (sizeof (PatternBranchPartOr)))) {
                pattern_error_code (FunctionCall, 1);
                return NULL;
        }
        if (!(or->left = memory_create (sizeof (PatternBranch)))) {
                branch_part_destroy_or (or);
                pattern_error_code (FunctionCall, 2);
                return NULL;
        }
        if (!(or->left->parts = list_create ())) {
                branch_part_destroy_or (or);
                pattern_error_code (FunctionCall, 3);
                return NULL;
        }
        if (!(or->right = memory_create (sizeof (PatternBranch)))) {
                branch_part_destroy_or (or);
                pattern_error_code (FunctionCall, 4);
                return NULL;
        }
        if (!(or->right->parts = list_create ())) {
                branch_part_destroy_or (or);
                pattern_error_code (FunctionCall, 5);
                return NULL;
        }
        or->base.type = PatternBranchPartTypeOr;
        *end = node->next;
        return (PatternBranchPart *)or;
}

static PatternBranchPart *branch_part_create_repeat (ListNode *node, ListNode **end)
{
        PatternBranchPartRepeat *repeat;
        
        if (!(repeat = memory_create (sizeof (PatternBranchPartRepeat)))) {
                pattern_error (FunctionCall);
                return NULL;
        }
        repeat->base.type = PatternBranchPartTypeRepeat;
        repeat->token = node->data;
        *end = node->next;
        return (PatternBranchPart *)repeat;
}

static PatternBranchPart *branch_part_create_range (ListNode *node, ListNode **end)
{
        PatternBranchPartRange *range;
        
        if (!(range = memory_create (sizeof (PatternBranchPartRange)))) {
                pattern_error (FunctionCall);
                return NULL;
        }
        range->base.type = PatternBranchPartTypeRange;
        range->token = node->data;
        *end = node->next;
        return (PatternBranchPart *)range;
}

static PatternBranchPart *branch_part_create_set (ListNode *node, ListNode **end)
{
        PatternBranchPartSet *set;
        
        if (!(set = memory_create (sizeof (PatternBranchPartSet)))) {
                pattern_error (FunctionCall);
                return NULL;
        }
        set->base.type = PatternBranchPartTypeSet;
        set->token = node->data;
        *end = node->next;
        return (PatternBranchPart *)set;
}

static PatternBranchPart *branch_part_create_value (ListNode *node, ListNode **end)
{
        PatternBranchPartValue *value;
        
        if (!(value = memory_create (sizeof (PatternBranchPartValue)))) {
                pattern_error (FunctionCall);
                return NULL;
        }
        value->base.type = PatternBranchPartTypeValue;
        value->token = node->data;
        *end = node->next;
        return (PatternBranchPart *)value;
}

static void branch_destroy (PatternBranch *branch)
{
        ListNode *node;

        if (!branch) {
                return;
        }
        for (node = list_first (branch->parts); node; node = node->next) {
                branch_part_destroy (node->data);
        }
        if (branch->parts) {
                list_destroy (branch->parts);
        }
        memory_destroy (branch);
}

static void branch_part_destroy (PatternBranchPart *part)
{
        if (!part) {
                return;
        }
        switch (part->type) {
        case PatternBranchPartTypeBranch:
                branch_part_destroy_branch ((PatternBranchPartBranch *)part);
                break;
        case PatternBranchPartTypeNot:
                branch_part_destroy_not ((PatternBranchPartNot *)part);
                break;
        case PatternBranchPartTypeOr:
                branch_part_destroy_or ((PatternBranchPartOr *)part);
                break;
        case PatternBranchPartTypeRepeat:
                branch_part_destroy_repeat ((PatternBranchPartRepeat *)part);
                break;
        case PatternBranchPartTypeRange:
                branch_part_destroy_range ((PatternBranchPartRange *)part);
                break;
        case PatternBranchPartTypeSet:
                branch_part_destroy_set ((PatternBranchPartSet *)part);
                break;
        case PatternBranchPartTypeValue:
                branch_part_destroy_value ((PatternBranchPartValue *)part);
                break;
        default:
                break;
        }
}

static void branch_part_destroy_branch (PatternBranchPartBranch *branch)
{
        if (!branch) {
                return;
        }
        branch_destroy (branch->branch);
        memory_destroy (branch);
}

static void branch_part_destroy_not (PatternBranchPartNot *not)
{
        if (!not) {
                return;
        }
        branch_part_destroy (not->part);
        memory_destroy (not);
}

static void branch_part_destroy_or (PatternBranchPartOr *or)
{
        if (!or) {
                return;
        }
        branch_destroy (or->left);
        branch_destroy (or->right);
        memory_destroy (or);
}

static void branch_part_destroy_repeat (PatternBranchPartRepeat *repeat)
{
        if (!repeat) {
                return;
        }
        branch_part_destroy (repeat->part);
        memory_destroy (repeat);
}

static void branch_part_destroy_range (PatternBranchPartRange *range)
{
        if (!range) {
                return;
        }
        memory_destroy (range);
}

static void branch_part_destroy_set (PatternBranchPartSet *set)
{
        if (!set) {
                return;
        }
        memory_destroy (set);
}

static void branch_part_destroy_value (PatternBranchPartValue *value)
{
        if (!value) {
                return;
        }
        memory_destroy (value);
}
