#!/bin/bash

# Copyright: (C) 2013 Istituto Italiano di Tecnologia (IIT)
# Author: Paul Fitzpatrick
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set -e

if [ -e "$YARP_DIR/lib/lua" ]; then
    export LUA_CPATH=";;;$YARP_DIR/lib/lua/?.so"
else
    export LUA_CPATH=";;;$PWD/?.so"
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Any test that requires yarpserver would go here:
# lua $DIR/test_that_requires_yarpserver.lua
