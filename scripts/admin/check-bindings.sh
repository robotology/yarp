#!/bin/bash

set -e

if [ "$YARP_ROOT" = "" ]; then
    YARP_ROOT="$PWD"
fi

if [ ! -e "$YARP_ROOT/conf/YarpVersion.cmake" ] ; then
    echo "Please run either from \$YARP_ROOT or with \$YARP_ROOT set"
    exit 1
fi

SUPPORTED_LANGUAGES="LUA PYTHON JAVA RUBY CSHARP PERL TCL"

if [ ! "$1" = "" ]; then
    SUPPORTED_LANGUAGES="$*"
fi

for lang in $SUPPORTED_LANGUAGES; do
    echo "Working on $lang"
    cd $YARP_ROOT
    mkdir -p bindings
    cd bindings
    dir="check_$lang"
    rm -rf $dir
    mkdir -p $dir
    cd $dir
    echo "* In $PWD"
    cmake -DCREATE_$lang=TRUE $YARP_ROOT/bindings
    make
done
