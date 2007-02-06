#!/bin/sh

set -x
aclocal
libtoolize --force
autoheader
automake --gnu --add-missing
autoconf
