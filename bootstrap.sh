#!/bin/sh

# you may have to modify this to match your versions
aclocal-1.8
autoheader
libtoolize
automake-1.8 --add-missing
autoconf

