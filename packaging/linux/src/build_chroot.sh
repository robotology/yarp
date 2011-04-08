#!/bin/bash

BUILD_DIR=$PWD

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

platform=$1
dir=$2

if [ "k$dir" = "k" ]; then
    echo "Call as: build_chroot.sh <platform> <dir>"
    exit 1
fi

source ./config_$platform.sh || {
	echo "No platform configuration file found"
	exit 1
}

if [ "k$PLATFORM_IS_UBUNTU" = "ktrue" ]; then
    PLATFORM_COMPONENTS="--components=main,universe"
fi

if [ ! -e $dir ]; then
    sudo debootstrap --variant=buildd $PLATFORM_COMPONENTS $platform $dir $PLATFORM_MIRROR || {
	sudo rm -rf chroot_$platform.fail; 
	sudo mv chroot_$platform chroot_$platform.fail 
	exit 1
    }
fi

(
	echo "export CHROOT_NAME='$platform'"
	echo "export CHROOT_DIR='$PWD/$dir'"
) > $BUILD_DIR/chroot_${platform}.sh
