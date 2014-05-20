clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/permute.c -o $PWD/lib.core/permute.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/object.c -o $PWD/lib.core/object.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/memory.c -o $PWD/lib.core/memory.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/random.c -o $PWD/lib.core/random.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/convert.c -o $PWD/lib.core/convert.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/topological.c -o $PWD/lib.core/topological.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/defines.c -o $PWD/lib.core/defines.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/combinations.c -o $PWD/lib.core/combinations.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/stopwatch.c -o $PWD/lib.core/stopwatch.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/error.c -o $PWD/lib.core/error.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/string.c -o $PWD/lib.core/string.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/pattern-whitespace.c -o $PWD/lib.core/pattern-whitespace.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/file.c -o $PWD/lib.core/file.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/tree.c -o $PWD/lib.core/tree.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/file-reader.c -o $PWD/lib.core/file-reader.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/tainio-tree.c -o $PWD/lib.core/tainio-tree.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/pattern-branch.c -o $PWD/lib.core/pattern-branch.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/pattern-token.c -o $PWD/lib.core/pattern-token.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/ascii.c -o $PWD/lib.core/ascii.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.core/list.c -o $PWD/lib.core/list.o
ar rsc $PWD/lib.core/lib.core.a $PWD/lib.core/permute.o $PWD/lib.core/object.o $PWD/lib.core/memory.o $PWD/lib.core/random.o $PWD/lib.core/convert.o $PWD/lib.core/topological.o $PWD/lib.core/defines.o $PWD/lib.core/combinations.o $PWD/lib.core/stopwatch.o $PWD/lib.core/error.o $PWD/lib.core/string.o $PWD/lib.core/pattern-whitespace.o $PWD/lib.core/file.o $PWD/lib.core/tree.o $PWD/lib.core/file-reader.o $PWD/lib.core/tainio-tree.o $PWD/lib.core/pattern-branch.o $PWD/lib.core/pattern-token.o $PWD/lib.core/ascii.o $PWD/lib.core/list.o 
ranlib $PWD/lib.core/lib.core.a
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.compile/compile.c -o $PWD/lib.compile/compile.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.compile/compile-project.c -o $PWD/lib.compile/compile-project.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.compile/compile-print.c -o $PWD/lib.compile/compile-print.o
ar rsc $PWD/lib.compile/lib.compile.a $PWD/lib.compile/compile.o $PWD/lib.compile/compile-project.o $PWD/lib.compile/compile-print.o $PWD/lib.core/lib.core.a 
ranlib $PWD/lib.compile/lib.compile.a
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/lib.test/test.c -o $PWD/lib.test/test.o
ar rsc $PWD/lib.test/lib.test.a $PWD/lib.test/test.o $PWD/lib.core/lib.core.a 
ranlib $PWD/lib.test/lib.test.a
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.compile/test-compile-project.c -o $PWD/app.test.lib.compile/test-compile-project.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.compile/test-compile.c -o $PWD/app.test.lib.compile/test-compile.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.compile/test.c -o $PWD/app.test.lib.compile/test.o
clang -O3 -lrt -lm $PWD/app.test.lib.compile/test-compile-project.o $PWD/app.test.lib.compile/test-compile.o $PWD/app.test.lib.compile/test.o $PWD/lib.test/lib.test.a $PWD/lib.compile/lib.compile.a $PWD/lib.core/lib.core.a -o $PWD/app.test.lib.compile/app.test.lib.compile 1>/dev/null
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.compile/main.c -o $PWD/app.compile/main.o
clang -O3 -lrt -lm $PWD/app.compile/main.o $PWD/lib.compile/lib.compile.a $PWD/lib.core/lib.core.a -o $PWD/app.compile/app.compile 1>/dev/null
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-list.c -o $PWD/app.test.lib.core/test-list.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-object.c -o $PWD/app.test.lib.core/test-object.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-memory.c -o $PWD/app.test.lib.core/test-memory.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-pattern-branch.c -o $PWD/app.test.lib.core/test-pattern-branch.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-file.c -o $PWD/app.test.lib.core/test-file.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-defines.c -o $PWD/app.test.lib.core/test-defines.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-stopwatch.c -o $PWD/app.test.lib.core/test-stopwatch.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-permute.c -o $PWD/app.test.lib.core/test-permute.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-pattern-token.c -o $PWD/app.test.lib.core/test-pattern-token.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-pattern-whitespace.c -o $PWD/app.test.lib.core/test-pattern-whitespace.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-tree.c -o $PWD/app.test.lib.core/test-tree.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-random.c -o $PWD/app.test.lib.core/test-random.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-combinations.c -o $PWD/app.test.lib.core/test-combinations.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-string.c -o $PWD/app.test.lib.core/test-string.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-tainio-tree.c -o $PWD/app.test.lib.core/test-tainio-tree.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-error.c -o $PWD/app.test.lib.core/test-error.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test.c -o $PWD/app.test.lib.core/test.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-ascii.c -o $PWD/app.test.lib.core/test-ascii.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-topological.c -o $PWD/app.test.lib.core/test-topological.o
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.test.lib.core/test-convert.c -o $PWD/app.test.lib.core/test-convert.o
clang -O3 -lrt -lm $PWD/app.test.lib.core/test-list.o $PWD/app.test.lib.core/test-object.o $PWD/app.test.lib.core/test-memory.o $PWD/app.test.lib.core/test-pattern-branch.o $PWD/app.test.lib.core/test-file.o $PWD/app.test.lib.core/test-defines.o $PWD/app.test.lib.core/test-stopwatch.o $PWD/app.test.lib.core/test-permute.o $PWD/app.test.lib.core/test-pattern-token.o $PWD/app.test.lib.core/test-pattern-whitespace.o $PWD/app.test.lib.core/test-tree.o $PWD/app.test.lib.core/test-random.o $PWD/app.test.lib.core/test-combinations.o $PWD/app.test.lib.core/test-string.o $PWD/app.test.lib.core/test-tainio-tree.o $PWD/app.test.lib.core/test-error.o $PWD/app.test.lib.core/test.o $PWD/app.test.lib.core/test-ascii.o $PWD/app.test.lib.core/test-topological.o $PWD/app.test.lib.core/test-convert.o $PWD/lib.test/lib.test.a $PWD/lib.core/lib.core.a -o $PWD/app.test.lib.core/app.test.lib.core 1>/dev/null
clang -std=gnu99 -pedantic -g -O3 -Wall -Wextra -Wconversion -Wformat-security -Werror -I$PWD -c $PWD/app.print_directory/main.c -o $PWD/app.print_directory/main.o
clang -O3 -lrt -lm $PWD/app.print_directory/main.o $PWD/lib.core/lib.core.a -o $PWD/app.print_directory/app.print_directory 1>/dev/null
