#!/bin/sh
find ./ -name *.o -delete
find ./ -name .*.swp -delete
find ./ -name *~ -delete
find ./ -name *.a -delete
rm -f app.compile/app.compile
rm -f app.print_directory/app.print_directory
rm -f app.test.lib.compile/app.test.lib.compile
rm -f app.test.lib.core/app.test.lib.core
