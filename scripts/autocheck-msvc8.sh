#!/bin/bash

# compiler/environment specific details
GEN="Visual Studio 8 2005"
CMAKE="/cygdrive/c/Program Files/CMake 2.4/bin/cmake.exe"
CMAKEOPTS="-DCREATE_GUIS:BOOL=TRUE -DCMAKE_COLOR_MAKEFILE:BOOL=FALSE"
NORMALIZER="cygpath -m"
DENORMALIZER="cygpath -w"
MAKE_CLEAN="echo do not bother cleaning"
MAKE_BUILD="devenv YARP.sln /Build Debug /Out makelog.txt"
MAKE_TEST="devenv YARP.sln /Build Debug /Project RUN_TESTS /Out makelog.txt"
#MAKE_CLEAN="nmake clean"
#MAKE_BUILD="nmake"
#MAKE_TEST="nmake test"
GO_OFFLINE="ipconfig /release"
GO_ONLINE="ipconfig /renew"

#  the rest should be independent


if [ "k$YARP_ROOTY" = "k" ]; then
    dir=`dirname $0`
    ydir="$PWD/$dir/.."
    export YARP_ROOT="$ydir"
    echo "Guessing YARP_ROOT to be something like $YARP_ROOT"
fi

SOURCE=`$DENORMALIZER $YARP_ROOT`


. $YARP_ROOT/scripts/config.sh
if [ "k$WEB_USER" = "k" ] ; then
    WEB_USER=`cat $YARP_ROOT/CVS/Root | sed "s/^:[^:]*://" | sed "s/[^a-z].*//"`
fi
echo USER is "$WEB_USER"

YARP1_INCLUDE="$YARP_ROOT/../yarp/include"

if [ -e $YARP1_INCLUDE ]; then
	echo "Found YARP1"
	p=`$NORMALIZER "$YARP1_INCLUDE"`
	CMAKEOPTS="$CMAKEOPTS -DCOEXIST_WITH_YARP1:BOOL=ON -DYARP1_INCLUDE_DIR:PATH=$p"
else
	echo "Did not find YARP1"
fi

curdir=`pwd`

while true; do

rm -f should_report.txt

(

(
cd $YARP_ROOT
cvs update -d > $curdir/cvslog.txt
cd $curdir
)
cat cvslog.txt | grep -v "cvs update" | egrep -v "^\? " | egrep -v "^M " | tee cvslog2.txt

if egrep "[a-zA-Z]" cvslog2.txt; then

echo Working in directory $SOURCE | tee should_report.txt

rm -f CMakeCache.txt
rm -f failure.txt
"$CMAKE" $CMAKEOPTS -G "$GEN" $SOURCE || ( echo YARP_AUTOCHECK cmake configure failed | tee failure.txt )

$MAKE_CLEAN || echo "make clean failed"

rm -f makelog.txt
if [ ! -e failure.txt ]; then
	$MAKE_BUILD || ( echo YARP_AUTOCHECK make build failed | tee failure.txt )
	if [ -e makelog.txt ]; then
	    cat makelog.txt
	fi
fi

echo "Regression tests not run" > testlog.txt
rm -f makelog.txt
if [ ! -e failure.txt ]; then
	# helpful to go offline
	$GO_OFFLINE
	(
	    $MAKE_TEST || ( echo YARP_AUTOCHECK make test failed | tee failure.txt )
	) | tee testlog.txt
	if [ -e makelog.txt ]; then
	    cat makelog.txt
	fi
	$GO_ONLINE
fi

cat cvslog.txt

if [ -e failure.txt ]; then
	echo YARP_AUTOCHECK at least one failure happened
else
    echo skip source build
#    touch build-source.txt
#    cat conf/build.txt | grep SOURCE | tee build-source-new.txt

#    cmp build-source-new.txt build-source.txt || (
#	echo "MAKING SOURCE PACKAGE"
#        # update packages
#	./scripts/make-source-package
#	./scripts/update-web-packages
#	echo "DONE MAKING SOURCE PACKAGE"
#    )
#    cp build-source-new.txt build-source.txt
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
	scp report-decor.txt $WEB_USER@$WEB_SERVER:$WEB_DIR/report-yarp2-msvc8.txt
fi

#./scripts/update-web-wrapper

sleep 600

done
