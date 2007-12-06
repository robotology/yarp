#!/bin/bash

# run as ./scripts/compile-binaries.sh

os=`uname`

if [ ! "k$os" = "kLinux" ] ; then
    echo only works on Linux
fi

cver_now=`cmake --version`
cver_min="cmake version 2.5-20071130"

sver=`( echo $cver_now; echo $cver_min ) | sort | head -n1`

if [ ! "k$sver" = "k$cver_min" ] ; then
    echo CMake minimum version usable is $cver_min
    echo CMake version on this machine is $cver_now
    echo I think that $cver_now is older than $cver_min
    echo "(of course, I am probably an idiot)"
    exit 1
fi

moved_cache=false

if [ -e CMakeCache.txt ] ; then
    mv CMakeCache.txt CMakeCache.txt.bak
    echo "Moving CMakeCache.txt out of the way to CMakeCache.txt.bak"
    moved_cache=true
fi

base=`pwd`
mkdir -p binary-helper
cd binary-helper
echo "working in directory binary-helper"

cmake -DBUILTIN_ACE=TRUE -DCMAKE_TOOLCHAIN_FILE=$base/scripts/mingw-crosscompiler-toolchain.cmake ..
make

cd $base
#if $moved_cache; then
if [ -e CMakeCache.txt.bak ]; then
    echo "Returning CMakeCache.txt"
    mv CMakeCache.txt.bak CMakeCache.txt
fi
