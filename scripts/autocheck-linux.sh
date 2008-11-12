#!/bin/bash

# run as ./scripts/autocheck-linux.sh

. ./scripts/config.sh
if [ "k$WEB_USER" = "k" ] ; then
    WEB_USER=`cat CVS/Root | sed "s/^:[^:]*://" | sed "s/[^a-z].*//"`
fi
echo USER is "$WEB_USER"

(
echo -n
#echo "SET(EXTERNAL_MODULES iCub)"
#echo "SET(iCub_DIR \"$PWD/../iCub\")"
) > conf/ExternalModules.cmake

#CMAKEOPTS="-DCREATE_GUIS:BOOL=TRUE -DCREATE_DEVICE_LIBRARY_BUILTINS:BOOL=TRUE -DCREATE_BUILTIN_DEVICE_TESTS:BOOL=TRUE -DENABLE_XSensMTx:BOOL=TRUE -DENABLE_dragonfly:BOOL=TRUE -DENABLE_ffmpeg:BOOL=TRUE -DENABLE_opencv_grabber:BOOL=TRUE -DENABLE_portaudio:BOOL=TRUE -DENABLE_microphone:BOOL=TRUE -DENABLE_esdMotionControl=TRUE"

CMAKEOPTS="-DCREATE_GUIS:BOOL=TRUE -DCREATE_DEVICE_LIBRARY_MODULES:BOOL=TRUE -DCREATE_BUILTIN_DEVICE_TESTS:BOOL=TRUE -DENABLE_yarpmod_ffmpeg:BOOL=TRUE -DENABLE_yarpmod_opencv:BOOL=TRUE -DENABLE_yarpmod_portaudio:BOOL=TRUE -DENABLE_yarpmod_microphone:BOOL=TRUE "

export YARP_ROOT=$PWD

rm -f should_report.txt

(

timeout 600 cvs update -d > cvslog.txt
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
else
    touch build-source.txt
    cat conf/build.txt | grep SOURCE | tee build-source-new.txt
    rm -rf debian-package-prev
    mv debian-package debian-package-prev || echo "no debian-package"
    cmp build-source-new.txt build-source.txt || (
        # update packages
	./scripts/make-source-package
	./scripts/update-web-packages
    )
    cp build-source-new.txt build-source.txt
    touch build-debian.txt
    cat conf/build.txt | grep DEBIAN | tee build-debian-new.txt
    cmp build-debian-new.txt build-debian.txt || (
        # update debian packages
	./scripts/make-debian
	./scripts/update-web-packages
    )
    cp build-debian-new.txt build-debian.txt
    ./scripts/autocheck-doc force
fi

else
	echo "Nothing new in CVS"
	./scripts/autocheck-doc
fi

) | tee report.txt

if [ -e should_report.txt ]; then
	date > report-decor.txt
	cat testlog.txt >> report-decor.txt
	echo >> report-decor.txt
	echo >> report-decor.txt
	cat report.txt >> report-decor.txt
	#scp report-decor.txt eshuy@yarp0.sf.net:www/report-yarp2-linux.txt
	timeout 600 scp report-decor.txt $WEB_USER@$WEB_SERVER:$WEB_DIR/report-yarp2-linux.txt
fi

