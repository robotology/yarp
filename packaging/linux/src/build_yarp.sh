#!/bin/bash

##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# Compile YARP within a chroot.
#
# Command line arguments: 
#   build_yarp.sh ${OPT_PLATFORM} ${OPT_CHROOT_DIRECTORY}
# Example:
#   build_yarp.sh etch chroot_etch
#
# Inputs:
#   settings.sh (general configuration)
#   chroot_${OPT_PLATFORM}.sh
#      records name and directory of a basic chroot
#
# Outputs:
#   ${OPT_CHROOT_DIRECTORY}/yarp-*.deb
#      a debian package containing YARP
#   ${OPT_CHROOT_DIRECTORY}/build_chroot
#      a chroot within which YARP has been compiled
#   yarp_${OPT_PLATFORM}.sh
#      records name and directory of chroot

BUILD_DIR=$PWD

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

source $BUNDLE_FILENAME || {
	echo "No bundle settings found: Tried '$BUNDLE_FILENAME'"
	exit 1
}

platform=$1
dir=$2

if [ "k$dir" = "k" ]; then
    echo "Call as: build_yarp.sh <platform> <dir>"
    exit 1
fi


# Load the configuration of the desired platform
source ./config_$platform.sh || {
	echo "No platform configuration file found"
	exit 1
}

# Load the configuration of a clean chroot
source chroot_${platform}.sh || {
	echo "Cannot find corresponding chroot"
	exit 1
}

mkdir -p $dir
cd $dir

# Create a chroot for building, if one does not already exist
if [ ! -e "build_chroot" ]; then
    sudo cp -R $CHROOT_DIR build_chroot || exit 1
fi

# Helper for running a command within the build chroot
function run_in_chroot {
    echo "Running [$2]"
    sudo chroot $1 bash -c "$2"
}

# Install basic dependencies
run_in_chroot build_chroot "yes | apt-get install libgsl0-dev libgtkmm-2.4-dev libace-dev subversion cmake dpkg wget" || exit 1
case "$platform" in
squeeze_i386|squeeze_amd64)
  run_in_chroot build_chroot "apt-get install -y libgoocanvasmm-dev"
  run_in_chroot build_chroot "echo 'deb http://backports.debian.org/debian-backports/ squeeze-backports main' >> /etc/apt/sources.list"
  run_in_chroot build_chroot "apt-get update && apt-get install -y -t squeeze-backports cmake"
  ;;
esac

if [ "k$TESTING" = "kTRUE" ]; then
	YARP_VERSION=$YARP_REVISION
	echo "yarp test revision $YARP_VERSION"
	run_in_chroot build_chroot "cd /tmp; test -e yarp2 || svn co -r $YARP_REVISION https://github.com/robotology/yarp/trunk yarp-$YARP_VERSION" || exit 1
	else
	echo "yarp tag $YARP_VERSION"
fi

# Fetch yarp from SVN
if [ ! -e build_chroot/tmp/yarp-$YARP_VERSION.done ]; then 
	echo "fetching yarp from SVN"
	# run_in_chroot build_chroot "cd /tmp; test -e yarp2 || svn co https://github.com/robotology/yarp/trunk/yarp2 yarp2" || exit 1
	run_in_chroot build_chroot "cd /tmp; test -e yarp-$YARP_VERSION  || svn co https://github.com/robotology/yarp/tags/v${YARP_VERSION} yarp-$YARP_VERSION" || exit 1
	run_in_chroot build_chroot "touch /tmp/yarp-$YARP_VERSION.done"
else
	echo "yarp already got!!"
fi

CHROOT_SRC=/tmp/yarp-$YARP_VERSION 
#run_in_chroot build_chroot "cd $CHROOT_SRC && svn up" || exit 1

# Prepare to build YARP.
CHROOT_BUILD=/tmp/yarp-$YARP_VERSION/build
run_in_chroot build_chroot "mkdir -p $CHROOT_BUILD" || exit 1
CMAKE=cmake

# Fetch cmake if version in repository is too old
if [ -e build_chroot/$CHROOT_BUILD/local_cmake ]; then
    CMAKE=`cd build_chroot/$CHROOT_BUILD/; echo cmake-*/bin/cmake`
else
    run_in_chroot build_chroot "cd $CHROOT_BUILD && cmake --version | grep ' 2\.[46]' && ( wget http://www.cmake.org/files/v2.8/cmake-2.8.12.1-Linux-i386.tar.gz && tar xzvf cmake-2.8.12.1-Linux-i386.tar.gz && touch local_cmake )"
fi

# If we downloaded cmake, we need to make sure we have 32-bit libraries
if [ -e build_chroot/$CHROOT_BUILD/local_cmake ]; then
    run_in_chroot build_chroot "yes | apt-get install ia32-libs"
    CMAKE=$CHROOT_BUILD/`cd build_chroot/$CHROOT_BUILD/; echo cmake-*/bin/cmake`
fi

# Go ahead and configure
run_in_chroot build_chroot "mkdir -p $CHROOT_BUILD && cd $CHROOT_BUILD && $CMAKE -DCREATE_GUIS=TRUE -DCREATE_YARPMANAGER_CONSOLE=TRUE -DCMAKE_INSTALL_PREFIX=/usr -DCREATE_SHARED_LIBRARY=TRUE -DCREATE_LIB_MATH=TRUE -DCREATE_IDLS=TRUE -DENABLE_yarpidl_thrift=TRUE -DCREATE_OPTIONAL_CARRIERS=TRUE -DENABLE_yarpcar_tcpros_carrier=TRUE -DENABLE_yarpcar_xmlrpc_carrier=TRUE -DENABLE_yarpcar_bayer_carrier=TRUE -DUSE_LIBDC1394=FALSE -DENABLE_yarpcar_priority_carrier=TRUE $CHROOT_SRC" || exit 1

#run_in_chroot build_chroot "mkdir -p $CHROOT_BUILD && cd $CHROOT_BUILD && $CMAKE -DCREATE_GUIS=TRUE -DCMAKE_INSTALL_PREFIX=/usr -DCREATE_SHARED_LIBRARY=TRUE -DCREATE_YARPSERVER3=TRUE -DCREATE_LIB_MATH=TRUE $CHROOT_SRC" || exit 1


# Go ahead and make
run_in_chroot build_chroot "cd $CHROOT_BUILD && make" || exit 1

# Go ahead and generate .deb
PACKAGE_DEPENDENCIES="libace-dev (>= 5.6), libgsl0-dev (>= 1.11), libgtkmm-2.4-dev (>= 2.14.1)"
case "$platform" in
squeeze_i386|squeeze_amd64)
  PACKAGE_DEPENDENCIES="$PACKAGE_DEPENDENCIES, libgoocanvasmm-dev"
  ;;
esac
run_in_chroot build_chroot "cd $CHROOT_BUILD && $CMAKE -DCPACK_GENERATOR='DEB' -DCPACK_DEBIAN_PACKAGE_VERSION=${YARP_VERSION}-${YARP_DEB_REVISION}~${PLATFORM_KEY} -DCPACK_PACKAGE_CONTACT='paul@robotrebuilt.com' -DCPACK_DEBIAN_PACKAGE_MAINTAINER='matteo.brunettini@iit.it' -DCPACK_DEBIAN_PACKAGE_DEPENDS:STRING='$PACKAGE_DEPENDENCIES' ." || exit 1
run_in_chroot build_chroot "cd $CHROOT_BUILD && rm -f *.deb && make package" || exit 1

# Rebuild .deb, because cmake 2.8.2 is broken, sigh
#   http://public.kitware.com/Bug/view.php?id=11020
run_in_chroot build_chroot "cd $CHROOT_BUILD && rm -rf deb *.deb" || exit 1
#PACK="deb/yarp-${YARP_VERSION}-${PLATFORM_KEY}-${PLATFORM_HARDWARE}"
YARP_PACKAGE_NAME="yarp-${YARP_VERSION}-${YARP_DEB_REVISION}~${PLATFORM_KEY}_${PLATFORM_HARDWARE}.deb"
PACK="deb/yarp-${YARP_VERSION}-${YARP_DEB_REVISION}~${PLATFORM_KEY}+${PLATFORM_HARDWARE}"
run_in_chroot build_chroot "cd $CHROOT_BUILD && mkdir -p $PACK/DEBIAN" || exit 1
run_in_chroot build_chroot "cd $CHROOT_BUILD && cp _CPack_Packages/Linux/DEB/yarp-*-Linux/control $PACK/DEBIAN" || exit 1
run_in_chroot build_chroot "cd $CHROOT_BUILD && cp _CPack_Packages/Linux/DEB/yarp-*-Linux/md5sums $PACK/DEBIAN" || exit 1
run_in_chroot build_chroot "cd $CHROOT_BUILD && cp -R _CPack_Packages/Linux/DEB/yarp-*-Linux/usr $PACK/usr" || exit 1
run_in_chroot build_chroot "cd $CHROOT_BUILD && dpkg -b $PACK $YARP_PACKAGE_NAME" || exit 1

# Copy .deb to somewhere easier to find
rm -f *.deb 2> /dev/null
cp build_chroot/$CHROOT_BUILD/$YARP_PACKAGE_NAME $YARP_PACKAGE_NAME || exit 1
#fname=`ls *.deb`

# Record settings
(
	echo "export YARP_PACKAGE_DIR='$PWD'"
	echo "export YARP_PACKAGE='$YARP_PACKAGE_NAME'"
) > $BUILD_DIR/yarp_${platform}.sh

# Report what we did
echo ".deb prepared, here:"
echo "  $PWD/$YARP_PACKAGE_NAME"
echo "To enter chroot used to build this .deb, run:"
echo "  sudo chroot $PWD/build_chroot"
