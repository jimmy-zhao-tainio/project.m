#ifndef pattern_path_match_h
#define pattern_path_match_h

#include <lib.core/pattern-path.h>

bool pattern_path_match (PatternPathTracker *tracker, const char *buffer, size_t buffer_length, size_t match_from, size_t *match_to);

#endif
