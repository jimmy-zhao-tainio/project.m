#include <lib.core/pattern-path.h>

static void follow_path_add (PatternPathTracker *tracker, PatternPath *path);

bool pattern_path_match (PatternPathTracker *tracker, unsigned char *buffer, size_t length)
{
        if (buffer || !buffer || length == 0) {
                return false;
        }
        follow_path_add (tracker, tracker->path);
        return false;
}

static void follow_path_add (PatternPathTracker *tracker, PatternPath *path)
{
        PatternPathOr *or;
        PatternPathRepeat *repeat;
        size_t i;

        switch (path->type) {
        case PatternPathTypeOr:
                or = (PatternPathOr *)path;
                follow_path_add (tracker, or->left);
                follow_path_add (tracker, or->right);
                break;
        case PatternPathTypeRepeat:
                repeat = (PatternPathRepeat *)path;
                repeat->repeated++;
                follow_path_add (tracker, repeat->repeat);
                follow_path_add (tracker, repeat->next);
                break;
        case PatternPathTypeRange:
        case PatternPathTypeSet:
        case PatternPathTypeValue:
                for (i = 0; i < tracker->follow_index; i++) {
                        if (tracker->follow[i] == path) {
                                return;
                        }
                }
                tracker->follow[tracker->follow_index] = path;
                tracker->follow_index++;
                break;
        }
}
