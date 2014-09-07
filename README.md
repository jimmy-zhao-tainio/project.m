project.m
=========

My recreational programming project.

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
                        Permutations of size_t integers.
                random
                        Random bytes or unsigned long long integers from /dev/urandom.
                stopwatch
                        Stopwatch to record time spent computing.
                tainio-tree
                        My solution to the reservoir problem. 
                        A self-balanced tree turned to output a range of natural numbers with uniformly 
                        random distribution. Suitable for large values of N and distributed computing.
                object
                        Unique identification for objects.
                file
                        Cached directory reading and file handling.
                combinations
                        Combinations of two.
                ascii
                        Help functions for ASCII.

                ... and more ...

lib.compile

        Automatic build system for C code.

lib.pattern

        Regular expression engine, better called "pattern" due to it's non standard syntax. 
        It uses a thompson construct and is immune to state explosions and regex attacks.
        Internally, pattern strings are turned into an intermediate branch structure, 
        before being transformed into a path structure which is used while matching.

        Supported syntax:

                ()      Grouping with parentheses. Can be nested.
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

lib.app

        Application support.
        Argument handling, supporting ordinal and named arguments (short and or long format), default values, 
        required and optional arguments.
        The usage is very simple, define an array of AppArguments and run app_arguments.
        
                AppArgument arguments[] = {
                        /* The arguments */
                };

                if (!app_arguments (argc, argv, arguments)) { ... }

        Help macros for AppArgument structures:

                ARGUMENT_ORDINAL_INTEGER (                       default, required, pointer, description)
                ARGUMENT_ORDINAL_STRING  (                       default, required, pointer, description)
                ARGUMENT_NAMED_BOOLEAN   (short_form, long_form, default, required, pointer, description)
                ARGUMENT_NAMED_INTEGER   (short_form, long_form, default, required, pointer, description)
                ARGUMENT_NAMED_STRING    (short_form, long_form, default, required, pointer, description)
                ARGUMENT_END

        If app_arguments does not succeed; use 'app_arguments_usage' to print usage help.

app.compile

        Automatic build system using lib.compile. A bit centric to this project.

app.pattern.replace

        Search and replace application using lib.pattern.

app.pattern.search

        Search using lib.pattern.

app.test.lib.compile
app.test.lib.core
app.test.lib.pattern
app.test.lib.app

        Unit-tests.
