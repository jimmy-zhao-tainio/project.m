gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/defines.c -o $PWD/lib.core/defines.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/random.c -o $PWD/lib.core/random.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/tree.c -o $PWD/lib.core/tree.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/error.c -o $PWD/lib.core/error.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/topological.c -o $PWD/lib.core/topological.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/file-writer.c -o $PWD/lib.core/file-writer.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/tainio-tree.c -o $PWD/lib.core/tainio-tree.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/ascii.c -o $PWD/lib.core/ascii.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/print.c -o $PWD/lib.core/print.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/combinations.c -o $PWD/lib.core/combinations.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/permute.c -o $PWD/lib.core/permute.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/file.c -o $PWD/lib.core/file.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/file-reader.c -o $PWD/lib.core/file-reader.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/list.c -o $PWD/lib.core/list.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/object.c -o $PWD/lib.core/object.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/memory.c -o $PWD/lib.core/memory.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/string.c -o $PWD/lib.core/string.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/stopwatch.c -o $PWD/lib.core/stopwatch.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.core/convert.c -o $PWD/lib.core/convert.o
ar rsc $PWD/lib.core/lib.core.a $PWD/lib.core/defines.o $PWD/lib.core/random.o $PWD/lib.core/tree.o $PWD/lib.core/error.o $PWD/lib.core/topological.o $PWD/lib.core/file-writer.o $PWD/lib.core/tainio-tree.o $PWD/lib.core/ascii.o $PWD/lib.core/print.o $PWD/lib.core/combinations.o $PWD/lib.core/permute.o $PWD/lib.core/file.o $PWD/lib.core/file-reader.o $PWD/lib.core/list.o $PWD/lib.core/object.o $PWD/lib.core/memory.o $PWD/lib.core/string.o $PWD/lib.core/stopwatch.o $PWD/lib.core/convert.o 
ranlib $PWD/lib.core/lib.core.a
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.graphics/structures.c -o $PWD/lib.graphics/structures.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.graphics/colors.c -o $PWD/lib.graphics/colors.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.graphics/canvas.c -o $PWD/lib.graphics/canvas.o
ar rsc $PWD/lib.graphics/lib.graphics.a $PWD/lib.graphics/structures.o $PWD/lib.graphics/colors.o $PWD/lib.graphics/canvas.o $PWD/lib.core/lib.core.a 
ranlib $PWD/lib.graphics/lib.graphics.a
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.app/arguments-validate.c -o $PWD/lib.app/arguments-validate.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.app/arguments-usage.c -o $PWD/lib.app/arguments-usage.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.app/arguments-default.c -o $PWD/lib.app/arguments-default.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.app/arguments.c -o $PWD/lib.app/arguments.o
ar rsc $PWD/lib.app/lib.app.a $PWD/lib.app/arguments-validate.o $PWD/lib.app/arguments-usage.o $PWD/lib.app/arguments-default.o $PWD/lib.app/arguments.o $PWD/lib.core/lib.core.a 
ranlib $PWD/lib.app/lib.app.a
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.test/test.c -o $PWD/lib.test/test.o
ar rsc $PWD/lib.test/lib.test.a $PWD/lib.test/test.o $PWD/lib.app/lib.app.a $PWD/lib.core/lib.core.a 
ranlib $PWD/lib.test/lib.test.a
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.graphics/test-canvas.c -o $PWD/app.test.lib.graphics/test-canvas.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.graphics/test.c -o $PWD/app.test.lib.graphics/test.o
gcc -lrt -lm -rdynamic $PWD/app.test.lib.graphics/test-canvas.o $PWD/app.test.lib.graphics/test.o $PWD/lib.test/lib.test.a $PWD/lib.app/lib.app.a $PWD/lib.graphics/lib.graphics.a $PWD/lib.core/lib.core.a -O0 -o $PWD/app.test.lib.graphics/app.test.lib.graphics -ldl 1>/dev/null
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.plugin/plugin.c -o $PWD/lib.plugin/plugin.o
ar rsc $PWD/lib.plugin/lib.plugin.a $PWD/lib.plugin/plugin.o $PWD/lib.core/lib.core.a 
ranlib $PWD/lib.plugin/lib.plugin.a
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.display.plugin/display-plugin.c -o $PWD/lib.display.plugin/display-plugin.o
ar rsc $PWD/lib.display.plugin/lib.display.plugin.a $PWD/lib.display.plugin/display-plugin.o $PWD/lib.plugin/lib.plugin.a $PWD/lib.graphics/lib.graphics.a $PWD/lib.core/lib.core.a 
ranlib $PWD/lib.display.plugin/lib.display.plugin.a
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.drawing/main.c -o $PWD/app.drawing/main.o
gcc -lrt -lm -rdynamic $PWD/app.drawing/main.o $PWD/lib.display.plugin/lib.display.plugin.a $PWD/lib.plugin/lib.plugin.a $PWD/lib.app/lib.app.a $PWD/lib.graphics/lib.graphics.a $PWD/lib.core/lib.core.a -O0 -o $PWD/app.drawing/app.drawing -ldl 1>/dev/null
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-list.c -o $PWD/app.test.lib.core/test-list.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-file-writer.c -o $PWD/app.test.lib.core/test-file-writer.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-stopwatch.c -o $PWD/app.test.lib.core/test-stopwatch.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-permute.c -o $PWD/app.test.lib.core/test-permute.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-file.c -o $PWD/app.test.lib.core/test-file.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-ascii.c -o $PWD/app.test.lib.core/test-ascii.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-memory.c -o $PWD/app.test.lib.core/test-memory.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-convert.c -o $PWD/app.test.lib.core/test-convert.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-string.c -o $PWD/app.test.lib.core/test-string.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-tree.c -o $PWD/app.test.lib.core/test-tree.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-object.c -o $PWD/app.test.lib.core/test-object.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-print.c -o $PWD/app.test.lib.core/test-print.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-random.c -o $PWD/app.test.lib.core/test-random.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-combinations.c -o $PWD/app.test.lib.core/test-combinations.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test.c -o $PWD/app.test.lib.core/test.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-topological.c -o $PWD/app.test.lib.core/test-topological.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-error.c -o $PWD/app.test.lib.core/test-error.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-tainio-tree.c -o $PWD/app.test.lib.core/test-tainio-tree.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-defines.c -o $PWD/app.test.lib.core/test-defines.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.core/test-file-reader.c -o $PWD/app.test.lib.core/test-file-reader.o
gcc -lrt -lm -rdynamic $PWD/app.test.lib.core/test-list.o $PWD/app.test.lib.core/test-file-writer.o $PWD/app.test.lib.core/test-stopwatch.o $PWD/app.test.lib.core/test-permute.o $PWD/app.test.lib.core/test-file.o $PWD/app.test.lib.core/test-ascii.o $PWD/app.test.lib.core/test-memory.o $PWD/app.test.lib.core/test-convert.o $PWD/app.test.lib.core/test-string.o $PWD/app.test.lib.core/test-tree.o $PWD/app.test.lib.core/test-object.o $PWD/app.test.lib.core/test-print.o $PWD/app.test.lib.core/test-random.o $PWD/app.test.lib.core/test-combinations.o $PWD/app.test.lib.core/test.o $PWD/app.test.lib.core/test-topological.o $PWD/app.test.lib.core/test-error.o $PWD/app.test.lib.core/test-tainio-tree.o $PWD/app.test.lib.core/test-defines.o $PWD/app.test.lib.core/test-file-reader.o $PWD/lib.test/lib.test.a $PWD/lib.app/lib.app.a $PWD/lib.core/lib.core.a -O0 -o $PWD/app.test.lib.core/app.test.lib.core -ldl 1>/dev/null
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.random.number/main.c -o $PWD/app.random.number/main.o
gcc -lrt -lm -rdynamic $PWD/app.random.number/main.o $PWD/lib.app/lib.app.a $PWD/lib.core/lib.core.a -O0 -o $PWD/app.random.number/app.random.number -ldl 1>/dev/null
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.compile/compile-print.c -o $PWD/lib.compile/compile-print.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.compile/compile-action.c -o $PWD/lib.compile/compile-action.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.compile/compile-project.c -o $PWD/lib.compile/compile-project.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.compile/compile.c -o $PWD/lib.compile/compile.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.compile/compile-link.c -o $PWD/lib.compile/compile-link.o
ar rsc $PWD/lib.compile/lib.compile.a $PWD/lib.compile/compile-print.o $PWD/lib.compile/compile-action.o $PWD/lib.compile/compile-project.o $PWD/lib.compile/compile.o $PWD/lib.compile/compile-link.o $PWD/lib.core/lib.core.a 
ranlib $PWD/lib.compile/lib.compile.a
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.compile/main.c -o $PWD/app.compile/main.o
gcc -lrt -lm -rdynamic $PWD/app.compile/main.o $PWD/lib.compile/lib.compile.a $PWD/lib.app/lib.app.a $PWD/lib.core/lib.core.a -O0 -o $PWD/app.compile/app.compile -ldl 1>/dev/null
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.pattern/pattern-token.c -o $PWD/lib.pattern/pattern-token.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.pattern/pattern-path.c -o $PWD/lib.pattern/pattern-path.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.pattern/pattern-branch.c -o $PWD/lib.pattern/pattern-branch.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.pattern/pattern-search.c -o $PWD/lib.pattern/pattern-search.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.pattern/pattern-whitespace.c -o $PWD/lib.pattern/pattern-whitespace.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/lib.pattern/pattern-path-match.c -o $PWD/lib.pattern/pattern-path-match.o
ar rsc $PWD/lib.pattern/lib.pattern.a $PWD/lib.pattern/pattern-token.o $PWD/lib.pattern/pattern-path.o $PWD/lib.pattern/pattern-branch.o $PWD/lib.pattern/pattern-search.o $PWD/lib.pattern/pattern-whitespace.o $PWD/lib.pattern/pattern-path-match.o $PWD/lib.core/lib.core.a 
ranlib $PWD/lib.pattern/lib.pattern.a
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.pattern.replace/main.c -o $PWD/app.pattern.replace/main.o
gcc -lrt -lm -rdynamic $PWD/app.pattern.replace/main.o $PWD/lib.pattern/lib.pattern.a $PWD/lib.app/lib.app.a $PWD/lib.core/lib.core.a -O0 -o $PWD/app.pattern.replace/app.pattern.replace -ldl 1>/dev/null
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.app/test-arguments-validate.c -o $PWD/app.test.lib.app/test-arguments-validate.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.app/test-arguments.c -o $PWD/app.test.lib.app/test-arguments.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.app/test-arguments-usage.c -o $PWD/app.test.lib.app/test-arguments-usage.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.app/test.c -o $PWD/app.test.lib.app/test.o
gcc -lrt -lm -rdynamic $PWD/app.test.lib.app/test-arguments-validate.o $PWD/app.test.lib.app/test-arguments.o $PWD/app.test.lib.app/test-arguments-usage.o $PWD/app.test.lib.app/test.o $PWD/lib.test/lib.test.a $PWD/lib.app/lib.app.a $PWD/lib.core/lib.core.a -O0 -o $PWD/app.test.lib.app/app.test.lib.app -ldl 1>/dev/null
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.pattern.search/main.c -o $PWD/app.pattern.search/main.o
gcc -lrt -lm -rdynamic $PWD/app.pattern.search/main.o $PWD/lib.pattern/lib.pattern.a $PWD/lib.app/lib.app.a $PWD/lib.core/lib.core.a -O0 -o $PWD/app.pattern.search/app.pattern.search -ldl 1>/dev/null
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.pattern/test-pattern-path.c -o $PWD/app.test.lib.pattern/test-pattern-path.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.pattern/test-pattern-token.c -o $PWD/app.test.lib.pattern/test-pattern-token.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.pattern/test-pattern-whitespace.c -o $PWD/app.test.lib.pattern/test-pattern-whitespace.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.pattern/test-pattern-search.c -o $PWD/app.test.lib.pattern/test-pattern-search.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.pattern/test.c -o $PWD/app.test.lib.pattern/test.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.pattern/test-pattern-path-match.c -o $PWD/app.test.lib.pattern/test-pattern-path-match.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.pattern/test-pattern-branch.c -o $PWD/app.test.lib.pattern/test-pattern-branch.o
gcc -lrt -lm -rdynamic $PWD/app.test.lib.pattern/test-pattern-path.o $PWD/app.test.lib.pattern/test-pattern-token.o $PWD/app.test.lib.pattern/test-pattern-whitespace.o $PWD/app.test.lib.pattern/test-pattern-search.o $PWD/app.test.lib.pattern/test.o $PWD/app.test.lib.pattern/test-pattern-path-match.o $PWD/app.test.lib.pattern/test-pattern-branch.o $PWD/lib.pattern/lib.pattern.a $PWD/lib.test/lib.test.a $PWD/lib.app/lib.app.a $PWD/lib.core/lib.core.a -O0 -o $PWD/app.test.lib.pattern/app.test.lib.pattern -ldl 1>/dev/null
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/plugin.display.x11/plugin.c -o $PWD/plugin.display.x11/plugin.o
gcc -lrt -lm -shared $PWD/plugin.display.x11/plugin.o $PWD/lib.graphics/lib.graphics.a $PWD/lib.core/lib.core.a -O0 -o $PWD/plugin.display.x11/plugin.display.x11.so 1>/dev/null
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.compile/test-compile-project.c -o $PWD/app.test.lib.compile/test-compile-project.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.compile/test.c -o $PWD/app.test.lib.compile/test.o
gcc -std=gnu99 -O0 -pedantic -g -Wall -Wextra -Wconversion -Wformat-security -Werror -fPIC -I$PWD -c $PWD/app.test.lib.compile/test-compile.c -o $PWD/app.test.lib.compile/test-compile.o
gcc -lrt -lm -rdynamic $PWD/app.test.lib.compile/test-compile-project.o $PWD/app.test.lib.compile/test.o $PWD/app.test.lib.compile/test-compile.o $PWD/lib.compile/lib.compile.a $PWD/lib.test/lib.test.a $PWD/lib.app/lib.app.a $PWD/lib.core/lib.core.a -O0 -o $PWD/app.test.lib.compile/app.test.lib.compile -ldl 1>/dev/null
