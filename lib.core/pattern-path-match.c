#include <lib.core/pattern-path.h>

static bool track (PatternPathTracker *tracker, PatternPath *path, unsigned char match);
static void track_push (PatternPathTracker *tracker, PatternPath *path);
static PatternPath *track_pop (PatternPathTracker *tracker);
static void repeat_reset (PatternPathTracker *tracker);

bool pattern_path_match (PatternPathTracker *tracker, char *buffer, size_t length, size_t *match_to)
{
        PatternPath *path;
        size_t buffer_index;
        size_t track_count;
        size_t track_index;
        
        repeat_reset (tracker);
        tracker->track_first = NULL;
        tracker->track_last = NULL;
        track_push (tracker, tracker->path);
        *match_to = 0;
        for (buffer_index = 0; buffer_index < length; buffer_index++) {
                track_count = tracker->track_count;
                for (track_index = 0; track_index < track_count; track_index++) {
                        if (!(path = track_pop (tracker))) {
                                return false;
                        }
                        if (track (tracker, path, (unsigned char)buffer[buffer_index])) {
                                *match_to = buffer_index + 1;
                        }
                }
        }
        while (track_pop (tracker)) {
        }

        if (*match_to != 0) {
                return true;
        }
        return false;
}

static bool track (PatternPathTracker *tracker, PatternPath *path, unsigned char match)
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
                if (track (tracker, or->left, match)) {
                        track (tracker, or->right, match);
                        return true;
                }
                return track (tracker, or->right, match);
        case PatternPathTypeRepeat:
                repeat = (PatternPathRepeat *)path;
                if (repeat->repeated == repeat->token.to) {
                        return track (tracker, repeat->next, match);
                }
                repeat->repeated++;
                if (repeat->repeated > repeat->token.from) {
                        if (track (tracker, repeat->next, match)) {
                                track (tracker, repeat->repeat, match);
                                return true;
                        }
                }
                return track (tracker, repeat->repeat, match);
        case PatternPathTypeRange:
                range = (PatternPathRange *)path;
                if (range->not == (range->token.from <= match && match <= range->token.to)) {
                        return false;
                }
                if (!range->next) {
                        return true;
                }
                track_push (tracker, range->next);
                return false;
        case PatternPathTypeSet:
                set = (PatternPathSet *)path;
                for (i = 0; i < set->token.values_length && match != set->token.values[i]; i++) {
                }
                if (set->not == (i != set->token.values_length)) {
                        return false;
                }
                if (!set->next) {
                        return true;
                }
                track_push (tracker, set->next);
                return false;
        case PatternPathTypeValue:
                value = (PatternPathValue *)path;
                if (value->not == (match == value->token.value)) {
                        return false;
                }
                if (!value->next) {
                        return true;
                }
                track_push (tracker, value->next);
                return false;
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

static void repeat_reset (PatternPathTracker *tracker)
{
        PatternPathRepeat *repeat;

        for (repeat = tracker->repeat; repeat; repeat = repeat->next_repeat) {
                repeat->repeated = 0;
        }
}
