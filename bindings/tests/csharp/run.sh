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

echo "We compile using mcs, the Mono C# Compiler"
for f in TestString TestStamp; do
    rm -rf $f.exe
    mcs -out:$f.exe $DIR/$f.cs $CSLIB/*.cs
    ./$f.exe
done
