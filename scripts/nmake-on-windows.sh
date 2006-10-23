#!/bin/bash

# run as ./scripts/nmake-on-windows.sh

SOURCE=`cygpath -w $PWD`
GEN="NMake Makefiles"
CMAKE="/cygdrive/c/Program Files/CMake 2.4/bin/cmake.exe"
CMAKEOPTS="-DCREATE_GUIS:BOOL=FALSE -DCREATE_DEVICE_LIBRARY:BOOL=TRUE"

echo Working in directory $SOURCE

rm -f CMakeCache.txt
rm -f failure.txt
"$CMAKE" $CMAKEOPTS -G "$GEN" . || ( echo YARP_AUTOCHECK cmake configure failed )

nmake 


