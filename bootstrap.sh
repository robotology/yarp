#!/bin/sh

# call with --force if needed

aclocal
autoheader
libtoolize $1
automake --add-missing
autoconf

