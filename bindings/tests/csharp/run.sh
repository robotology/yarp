#!/bin/bash

# Copyright: (C) 2014 iCub Facility
# Author: Paul Fitzpatrick
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set -e

export CSLIB="$PWD"
if [ ! -e $PWD/Network.cs ]; then
    export CSLIB="$YARP_DIR/lib/csharp/"
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "HELLO $CSLIB // $DIR"

echo "We compile using gmcs, the Mono C# Compiler"
rm -rf test_string.exe
gmcs -out:test_string.exe $DIR/TestString.cs $CSLIB/*.cs
./test_string.exe
