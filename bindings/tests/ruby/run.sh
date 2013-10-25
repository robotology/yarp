#!/bin/bash

# Copyright: (C) 2013 iCub Facility
# Author: Paul Fitzpatrick
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set -e

if [ ! "$YARP_DIR" = "" ]; then
    export RUBYLIB="$YARP_DIR/lib/ruby/"
else
    export RUBYLIB="$PWD"
fi
export LD_LIBRARY_PATH="$RUBYLIB:$LD_LIBRARY_PATH"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

ruby $DIR/test_string.rb
