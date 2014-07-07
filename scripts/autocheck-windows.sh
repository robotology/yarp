#!/bin/bash

# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Lorenzo Natale, Giorgio Metta
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# run as ./scripts/autocheck-windows.sh

. ./scripts/config.sh
#if [ "k$WEB_USER" = "k" ] ; then
#    WEB_USER=`cat CVS/Root | sed "s/^:[^:]*://" | sed "s/[^a-z].*//"`
#fi
echo USER is "$WEB_USER"

SOURCE=`cygpath -w $PWD`
GEN="NMake Makefiles"
CMAKE="/cygdrive/c/Program Files/CMake 2.8/bin/cmake.exe"
CMAKEOPTS="-DCREATE_GUIS:BOOL=TRUE -DCMAKE_COLOR_MAKEFILE:BOOL=FALSE -DCREATE_YARPSERVER3:BOOL=TRUE"

YARP1_INCLUDE="$PWD/../yarp/include"

if [ -e $YARP1_INCLUDE ]; then
	echo "Found YARP1"
	p=`cygpath -m $YARP1_INCLUDE`
	CMAKEOPTS="$CMAKEOPTS -DCOEXIST_WITH_YARP1:BOOL=ON -DYARP1_INCLUDE_DIR:PATH=$p"
else
	echo "Did not find YARP1"
fi

rm -f should_report.txt

(

timeout 600 svn update > svnlog.txt
cat svnlog.txt | grep -v "svn update " | egrep -v "^\? " | egrep -v "^M " | tee svnlog2.txt

if egrep "[a-zA-Z]" svnlog2.txt; then

echo Working in directory $SOURCE | tee should_report.txt

rm -f CMakeCache.txt
rm -f failure.txt
"$CMAKE" $CMAKEOPTS -G "$GEN" $SOURCE || ( echo YARP_AUTOCHECK cmake configure failed | tee failure.txt )

nmake clean || echo "nmake clean failed"

if [ ! -e failure.txt ]; then
	nmake || ( echo YARP_AUTOCHECK nmake compile failed | tee failure.txt )
fi

echo "Regression tests not run" > testlog.txt
if [ ! -e failure.txt ]; then
	# helpful to go offline
	ipconfig /release
	(
	    nmake test || ( echo YARP_AUTOCHECK nmake regression failed | tee failure.txt )
	) | tee testlog.txt
	ipconfig /renew
fi

cat svnlog.txt

if [ -e failure.txt ]; then
	echo YARP_AUTOCHECK at least one failure happened
else
    touch build-source.txt
    cat conf/build.txt | grep SOURCE | tee build-source-new.txt

    cmp build-source-new.txt build-source.txt || (
	echo "MAKING SOURCE PACKAGE"
        # update packages
	./scripts/make-source-package
	./scripts/update-web-packages
	echo "DONE MAKING SOURCE PACKAGE"
    )
    cp build-source-new.txt build-source.txt
fi

else
	echo "Nothing new in svn"
fi

) 2>&1 | tee report.txt

if [ -e should_report.txt ]; then
	date > report-decor.txt
	cat testlog.txt >> report-decor.txt
	echo >> report-decor.txt
	echo >> report-decor.txt
	cat report.txt >> report-decor.txt
	#scp report-decor.txt eshuy@yarp0.sf.net:www/report-yarp2-windows.txt
	scp report-decor.txt $WEB_USER@$WEB_SERVER:$WEB_DIR/report-yarp2-windows.txt
fi

#./scripts/update-web-wrapper

