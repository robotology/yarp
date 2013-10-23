#!/bin/bash

# Copyright: (C) 2013 iCub Facility
# Author: Paul Fitzpatrick
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set -e

if [ ! "$YARP_DIR" = "" ]; then
    export LD_LIBRARY_PATH=$YARP_DIR/lib/python:$LD_LIBRARY_PATH
    export PYTHONPATH=$YARP_DIR:$PYTHONPATH
else
    export LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH
    export PYTHONPATH=$PWD:$PYTHONPATH
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

python="python"

echo "detecting active python version..."
for ver in 2.5 2.6 2.7 3 3.1 3.2 3.3 3.4 3.5 3.6 4; do
    {
	python$ver $DIR/test_load.py && python="python$ver" || echo -n
    } > /dev/null 2>&1
done
echo "using '$python'"

$python $DIR/test_string.py
