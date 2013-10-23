#!/bin/bash

# Copyright: (C) 2013 iCub Facility
# Author: Paul Fitzpatrick
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set -e

if [ ! "$YARP_DIR" = "" ]; then
    export LUA_CPATH=";;;$YARP_DIR/lib/lua/?.so"
else
    export LUA_CPATH=";;;$PWD/?.so"
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

lua $DIR/test_string.lua
