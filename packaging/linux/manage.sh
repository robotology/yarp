#!/bin/bash

if [ -e manage.sh ]; then
	echo "Please call from a build directory"
	exit 1
fi

declare -A check_for_associative_arrays || (
	echo "Update cygwin bash version"
	exit 1
)

# set up build and source directory
BUILD_DIR=$PWD
relative_dir="`dirname $0`"
cd "$relative_dir"
SOURCE_DIR=$PWD

# load bundle settings
if [ "k$1" = "k" ]; then
	echo "Please specify a bundle name.  One of these:"
	echo `cd conf; ls -1 bundle*.sh | sed "s/\.sh//"`
	exit 1
fi
BUNDLE_NAME="$1"
BUNDLE_FILENAME="conf/$1.sh"
if [ ! -e "$BUNDLE_FILENAME" ]; then
	echo "Cannot find $BUNDLE_FILENAME"
	exit 1
fi
source $BUNDLE_FILENAME

# remember command
(
	echo "default:"
	echo -e "\t$0 $1\n"
) > $BUILD_DIR/Makefile

# load compiler settings
source $SOURCE_DIR/conf/compilers.sh || exit 1
if [ -e $SOURCE_DIR/conf/compilers_local.sh ]; then
	source $SOURCE_DIR/conf/compilers_local.sh || exit 1
fi

cd $BUILD_DIR
(
	echo "export SOURCE_DIR='$SOURCE_DIR'"
	echo "export BUNDLE_NAME='$BUNDLE_NAME'"
	echo "export BUNDLE_FILENAME='$SOURCE_DIR/$BUNDLE_FILENAME'"
) > settings.sh

if sudo which debootstrap; then
    echo "Good, have debootstrap"
else
    sudo apt-get install debootstrap
fi

PLATFORM_SCRIPTS=/usr/share/debootstrap/scripts
ls $PLATFORM_SCRIPTS

for platform in $PLATFORMS; do
    if [ ! -e "$PLATFORM_SCRIPTS/$platform" ]; then
	echo "Do not know how to make $platform"
	exit 1
    fi
    echo "Preparing $platform"
    (
	echo "chroot_$platform:"
	echo -e "\t$SOURCE_DIR/src/build_chroot.sh $platform chroot_$platform\n"
    ) >> $BUILD_DIR/Makefile
    #  --components=main,universe
    # http://ubuntu.media.mit.edu/ubuntu/
done
