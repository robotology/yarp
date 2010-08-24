#!/bin/bash

# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Lorenzo Natale, Giorgio Metta
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# run as ./scripts/autocheck-generic-unix.sh

. ./scripts/config.sh
if [ "k$WEB_USER" = "k" ] ; then
    WEB_USER=`cat CVS/Root | sed "s/^:[^:]*://" | sed "s/[^a-z].*//"`
fi
key=`uname`
echo USER is "$WEB_USER" and KEY is "$key"

CMAKEOPTS="-DCREATE_GUIS:BOOL=TRUE -DCREATE_DEVICE_LIBRARY:BOOL=TRUE"

export YARP_ROOT=$PWD

rm -f should_report.txt

(

cvs update -d > cvslog.txt
cat cvslog.txt | grep -v "cvs update" | egrep -v "^\? " | egrep -v "^M " | tee cvslog2.txt

if egrep "[a-zA-Z]" cvslog2.txt; then

SOURCE=$PWD

echo Working in directory $SOURCE | tee should_report.txt

rm -f CMakeCache.txt
rm -f failure.txt
echo running cmake $CMAKEOPTS $SOURCE
cmake $CMAKEOPTS $SOURCE || ( echo YARP_AUTOCHECK cmake configure failed | tee failure.txt )
echo repeating cmake $SOURCE
cmake $SOURCE
# there seems to be a bug in cmake 2.5 with add_definitions 

make clean || echo "make clean failed"

if [ ! -e failure.txt ]; then
	make || ( echo YARP_AUTOCHECK make compile failed | tee failure.txt )
fi

echo "Regression tests not run" > testlog.txt
if [ ! -e failure.txt ]; then
    (
	rm -f src/libYARP_dev/harness/*.dox
	make test || ( echo YARP_AUTOCHECK make regression failed | tee failure.txt )
    ) 2>&1 | tee testlog.txt
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
	cat testlog.txt >> report-decor.txt
	echo >> report-decor.txt
	echo >> report-decor.txt
	cat report.txt >> report-decor.txt
	scp report-decor.txt $WEB_USER@$WEB_SERVER:$WEB_DIR/report-yarp2-$key.txt
fi

