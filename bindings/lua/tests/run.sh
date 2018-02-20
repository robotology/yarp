#!/bin/bash

# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

set -e

if [ -e "$YARP_DIR/lib/lua" ]; then
    export LUA_CPATH=";;;$YARP_DIR/lib/lua/?.so"
else
    export LUA_CPATH=";;;$PWD/?.so"
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

lua $DIR/test_string.lua
