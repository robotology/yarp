#!/bin/bash

# Copyright: (C) 2013 iCub Facility
# Author: Paul Fitzpatrick
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set -e

if [ -e "$YARP_DIR/lib/ruby" ]; then
    export RUBYLIB="$YARP_DIR/lib/ruby/:$PWD"
else
    export RUBYLIB="$PWD"
fi
export LD_LIBRARY_PATH="$RUBYLIB:$LD_LIBRARY_PATH"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "LD_LIBRARY_PATH: $LD_LIBRARY_PATH"
echo "RUBYLIB: $RUBYLIB"
echo "== ls{"
ls
echo "== ls}"
if [ -e yarp.so ]; then
    echo "ldd yarp.so"
    ldd yarp.so
    echo "ruby -v"
    ruby -v
fi

ruby $DIR/test_string.rb
