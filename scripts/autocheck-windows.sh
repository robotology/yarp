#!/bin/bash

# run as ./scripts/autocheck-windows.sh

#cvs update -d

SOURCE=`cygpath -w $PWD`
GEN="NMake Makefiles"
CMAKE="/cygdrive/c/Program Files/CMake 2.4/bin/cmake.exe"

while true; do

(

echo Working in directory $SOURCE

rm -f CMakeCache.txt
rm -f failure.txt
"$CMAKE" -G "$GEN" $SOURCE || ( echo YARP_AUTOCHECK cmake configure failed | tee failure.txt )

if [ ! -e failure.txt ]; then
	nmake || ( echo YARP_AUTOCHECK nmake compile failed | tee failure.txt )
fi

if [ ! -e failure.txt ]; then
	nmake test || ( echo YARP_AUTOCHECK nmake regression failed | tee failure.txt )
fi

if [ -e failure.txt ]; then
	echo YARP_AUTOCHECK at least one failure happened
fi


) | tee report.txt

if [ ! -e report-prev.txt ]; then
	echo | report-prev.txt
fi

cmp report.txt report-prev.txt || (
	cp report.txt report-prev.txt
	date > report-decor.txt
	cat report.txt >> report-decor.txt
	scp report-decor.txt eshuy@yarp0.sf.net:www/report-yarp2-windows.txt
)

sleep 60

done
