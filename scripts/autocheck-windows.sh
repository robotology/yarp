#!/bin/bash

# run as ./scripts/autocheck-windows.sh

SOURCE=`cygpath -w $PWD`
GEN="NMake Makefiles"
CMAKE="/cygdrive/c/Program Files/CMake 2.4/bin/cmake.exe"
CMAKEOPTS="-DCREATE_GUIS:BOOL=FALSE -DCREATE_DEVICE_LIBRARY:BOOL=TRUE"

while true; do

rm -f should_report.txt

(

cvs update -d > cvslog.txt
cat cvslog.txt | grep -v "cvs update" | egrep -v "^\? " | egrep -v "^M " | tee cvslog2.txt

if egrep "[a-zA-Z]" cvslog2.txt; then

echo Working in directory $SOURCE | tee should_report.txt

rm -f CMakeCache.txt
rm -f failure.txt
"$CMAKE" $CMAKEOPTS -G "$GEN" $SOURCE || ( echo YARP_AUTOCHECK cmake configure failed | tee failure.txt )

nmake clean || echo "nmake clean failed"

if [ ! -e failure.txt ]; then
	nmake || ( echo YARP_AUTOCHECK nmake compile failed | tee failure.txt )
fi

if [ ! -e failure.txt ]; then
	nmake test || ( echo YARP_AUTOCHECK nmake regression failed | tee failure.txt )
fi

cat cvslog.txt

if [ -e failure.txt ]; then
	echo YARP_AUTOCHECK at least one failure happened
fi

else
	echo "Nothing new in CVS"
fi

) | tee report.txt

if [ -e should_report.txt ]; then
	date > report-decor.txt
	cat report.txt >> report-decor.txt
	scp report-decor.txt eshuy@yarp0.sf.net:www/report-yarp2-windows.txt
fi

sleep 600

done
