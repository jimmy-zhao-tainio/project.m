#include <lib.core/pattern-path.h>

static void track (PatternPathTracker *tracker, PatternPath *path, unsigned char match);
static void track_push (PatternPathTracker *tracker, PatternPath *path);
static PatternPath *track_pop (PatternPathTracker *tracker);

bool pattern_path_match (PatternPathTracker *tracker, unsigned char *buffer, size_t length)
{
        PatternPath *path;
        size_t buffer_index;
        size_t track_count;
        size_t track_index;

        if (buffer || !buffer || length == 0) {
                return false;
        }
        tracker->track_first = NULL;
        tracker->track_last = NULL;
        track_push (tracker, tracker->path);
        for (buffer_index = 0; buffer_index < length; buffer_index++) {
                track_count = tracker->track_count;
                for (track_index = 0; track_index < track_count; track_index++) {
                        if (!(path = track_pop (tracker))) {
                                return false;
                        }
                        track (tracker, path, buffer[buffer_index]);
                }
        }
        return false;
}

static void track (PatternPathTracker *tracker, PatternPath *path, unsigned char match)
{
        PatternPathOr *or;
        PatternPathRepeat *repeat;
        PatternPathRange *range;
        PatternPathSet *set;
        PatternPathValue *value;
        size_t i;

        switch (path->type) {
        case PatternPathTypeOr:
                or = (PatternPathOr *)path;
                track (tracker, or->left, match);
                track (tracker, or->right, match);
                break;
        case PatternPathTypeRepeat:
                repeat = (PatternPathRepeat *)path;
                if (repeat->repeated == repeat->token.to) {
                        track (tracker, repeat->next, match);
                        break;
                }
                repeat->repeated++;
                if (repeat->repeated > repeat->token.from) {
                        track (tracker, repeat->next, match);
                        break;
                }
                track (tracker, repeat->repeat, match);
                break;
        case PatternPathTypeRange:
                range = (PatternPathRange *)path;
                if (range->not == (range->token.from <= match && match <= range->token.to)) {
                        break;
                }
                track_push (tracker, range->next);
                break;
        case PatternPathTypeSet:
                set = (PatternPathSet *)path;
                for (i = 0; i < set->token.values_length && match != set->token.values[i]; i++) {
                }
                if (set->not == (i != set->token.values_length)) {
                        break;
                }
                track_push (tracker, set->next);
                break;
        case PatternPathTypeValue:
                value = (PatternPathValue *)path;
                if (value->not == (match == value->token.value)) {
                        break;
                }
                track_push (tracker, value->next);
                break;
        }
}

static void track_push (PatternPathTracker *tracker, PatternPath *path)
{
        if (path == NULL) {
                return;
        }
        if (path->track_pushed == true) {
                return;
        }
        path->track_pushed = true;
        if (!tracker->track_first) {
                tracker->track_first = path;
                tracker->track_last = path;
                path->track_previous = NULL;
                path->track_next = NULL;
        }
        else {
                tracker->track_last->track_next = path;
                path->track_previous = tracker->track_last;
                path->track_next = NULL;
                tracker->track_last = path;
        }
        tracker->track_count++;
}

static PatternPath *track_pop (PatternPathTracker *tracker)
{
        PatternPath *path;

        if (!tracker->track_first) {
                return NULL;
        }
        path = tracker->track_first;
        path->track_pushed = false;
        tracker->track_first = tracker->track_first->track_next;
        if (tracker->track_first) {
                tracker->track_first->track_previous = NULL;
        }
        else {
                tracker->track_last = NULL;
        }
        tracker->track_count--;
        return path;
}
