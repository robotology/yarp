#!/bin/bash

BUILD_DIR=$PWD

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

platform=$1
dir=$2

if [ "k$dir" = "k" ]; then
    echo "Call as: build_yarp.sh <platform> <dir>"
    exit 1
fi

source chroot_${platform}.sh || {
	echo "Cannot find corresponding chroot"
	exit 1
}

mkdir -p $dir
cd $dir

if [ ! -e "build_chroot" ]; then
    sudo cp -R $CHROOT_DIR build_chroot || exit 1
fi

function run_in_chroot {
    echo "Running [$2]"
    sudo chroot $1 bash -c "$2"
}

run_in_chroot build_chroot "yes | apt-get install libgsl0-dev libgtkmm-2.4-dev libace-dev subversion cmake" || exit 1
run_in_chroot build_chroot "cd /tmp; test -e yarp2 || svn co https://yarp0.svn.sourceforge.net/svnroot/yarp0/trunk/yarp2 yarp2" || exit 1
CHROOT_SRC=/tmp/yarp2
CHROOT_BUILD=/tmp/yarp2/build
run_in_chroot build_chroot "cd $CHROOT_SRC && svn up" || exit 1
run_in_chroot build_chroot "mkdir -p $CHROOT_BUILD && cd $CHROOT_BUILD && cmake -DCREATE_GUIS=TRUE -DCREATE_SHARED_LIBRARY=TRUE -DCREATE_YARPSERVER3=TRUE -DCREATE_LIB_MATH=TRUE $CHROOT_SRC" || exit 1
run_in_chroot build_chroot "cd $CHROOT_BUILD && make" || exit 1
run_in_chroot build_chroot "cd $CHROOT_BUILD && cmake -DCPACK_GENERATOR='DEB' -DCPACK_PACKAGE_CONTACT='paul@robotrebuilt.com' -DCPACK_DEBIAN_PACKAGE_MAINTAINER='paul@robotrebuilt.com' -DCPACK_DEBIAN_PACKAGE_DEPENDS:STRING='libace-dev (>= 5.6), libgsl0-dev (>= 1.11), libgtkmm-2.4-dev (>= 2.14.1)' ." || exit 1
run_in_chroot build_chroot "cd $CHROOT_BUILD && rm -f *.deb && make package" || exit 1

rm -f *.deb
cp build_chroot/$CHROOT_BUILD/yarp-*.deb . || exit 1
fname=`ls *.deb`

(
	echo "export YARP_PACKAGE_DIR='$PWD'"
	echo "export YARP_PACKAGE='$fname'"
) > $BUILD_DIR/yarp_${platform}.sh
