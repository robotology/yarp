#!/bin/bash

# run as ./scripts/autocheck-linux.sh

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
cmake $CMAKEOPTS $SOURCE || ( echo YARP_AUTOCHECK cmake configure failed | tee failure.txt )

make clean || echo "make clean failed"

if [ ! -e failure.txt ]; then
	make || ( echo YARP_AUTOCHECK make compile failed | tee failure.txt )
fi

if [ ! -e failure.txt ]; then
	make test || ( echo YARP_AUTOCHECK make regression failed | tee failure.txt )
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
	scp report-decor.txt eshuy@yarp0.sf.net:www/report-yarp2-linux.txt
fi

sleep 600

done
