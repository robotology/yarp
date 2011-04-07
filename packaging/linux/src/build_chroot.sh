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
fi

if [ ! -e $dir ]; then
    sudo debootstrap --variant=buildd $platform $dir || ( 
	sudo rm -rf chroot_$platform.fail; 
	sudo mv chroot_$platform chroot_$platform.fail 
	exit 1
    )
fi

(
echo "yes | apt-get install libgsl0-dev libgtkmm-2.4-dev libace-dev subversion"
) | sudo chroot $dir
