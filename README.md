project.m
=========

lib.core

        Shared library containing various basic programming concepts.

                memory
                        Accountable memory handler. Supports commit limits and stored block sizes.
                string
                        Functions to manipulate C-strings.
                error
                        Error handling with enums.
                list
                        Double linked list.
                tree
                        Self-balanced tree.
                topological
                        Topological graph sorting.
                defines
                        Overflow checks and common includes.
                convert
                        String to integer conversion.
                permute
                        Permutions of size_t integers.
                random
                        Random bytes or unsigned long long integers from /dev/urandom.
                stopwatch
                        Stopwatch to record time spent computing.
                tainio-tree
                        My solution to the reservoir problem. 
                        A self-balanced tree turned to output a range of natural numbers having uniformly 
                        random distribution. Suitable for large values of N and distributed computation.
                object
                        Unique identification for objects.
                file
                        Cached directory reading and file handling.
                combinations
                        Combinations of two.
                ascii
                        Help functions for ASCII.

lib.compile

        Automatic build system for C code.

lib.pattern

        Regular expression engine, better called "pattern" due to it's non standard syntax. 
        It uses a thompson construct and is impervious to state explosions and regex attacks.
        Internally, pattern strings are first turned into an intermediate branch structure, 
        before being transformed into a path structure which is used while matching.

        Supported syntax:

                ()      Grouping with parentheses, also nested.
                !       Not operator for ranges, sets and values.
                |       Or operator.
                {m-n}   Repeat operator, m and n being optional.
                [m-n]   Range of values, m and n being optional.
                <...>   Set of values.
                value   Character or escaped character:
                        \B for 8-bit binary
                        \o for octal
                        \x for hex
                        \  for space
                        \c where 'c' is any character escapable by the C language.
                        \s where 's' is any character used by the pattern syntax.

lib.test

        Unit-testing, checks for memory leaks and more.

app.compile

        Application using lib.compile.

app.pattern.replace

        Search and replace application using lib.pattern.

app.pattern.search

        Search using lib.pattern.

app.test.lib.compile
        
        Unit-tests.

app.test.lib.core

        Unit-tests.

app.test.lib.pattern

        Unit-tests.
