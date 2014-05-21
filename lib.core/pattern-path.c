#include <lib.core/pattern-path.h>
#include <lib.core/error.h>

bool pattern_path_buffer_size (PatternBranch *branch, size_t *size)
{
        if (!branch) {
                error_code (InvalidArgument, 1);
                return false;
        }
        if (!size) {
                error_code (InvalidArgument, 2);
                return false;
        }
        size = 0;
        return false;
}
