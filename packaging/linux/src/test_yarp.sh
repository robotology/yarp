#!/bin/bash

BUILD_DIR=$PWD

source ./settings.sh || {
	echo "No settings.sh found, are we in the build directory?"
	exit 1
}

platform=$1

if [ "k$platform" = "k" ]; then
    echo "Call as: test_yarp.sh <platform>"
    exit 1
fi

source chroot_${platform}.sh || {
	echo "Cannot find corresponding chroot"
	exit 1
}

source yarp_${platform}.sh || {
	echo "Cannot find corresponding YARP package"
	exit 1
}

function run_in_chroot {
    echo "Running [$2]"
    sudo chroot $1 bash -c "$2"
}

cd $YARP_PACKAGE_DIR
test -e $YARP_PACKAGE || exit 1

if [ ! -e test_chroot ]; then
    sudo cp -R $CHROOT_DIR test_chroot || exit 1
fi

sudo cp yarp-*.deb test_chroot/tmp || exit 1
run_in_chroot test_chroot "yes | apt-get remove yarp"
run_in_chroot test_chroot "yes | apt-get install gdebi-core" || exit 1
run_in_chroot test_chroot "cd tmp && gdebi --non-interactive *.deb" || exit 1

echo "To enter chroot, run"
echo "  sudo chroot $YARP_PACKAGE_DIR/test_chroot"
