#!/bin/bash

if [ -e manage.sh ]; then
	echo "Please call from a build directory"
	exit 1
fi

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
	echo -e "\t$0 $1"
) > $BUILD_DIR/Makefile

# load compiler settings
source $SOURCE_DIR/conf/compilers.sh || exit 1
if [ -e $SOURCE_DIR/conf/compilers_local.sh ]; then
	source $SOURCE_DIR/conf/compilers_local.sh || exit 1
fi

export cmake_path=`cygpath --unix "$cmake_path"`

cd $BUILD_DIR
(
	echo "export SOURCE_DIR='$SOURCE_DIR'"
	echo "export BUNDLE_NAME='$BUNDLE_NAME'"
	echo "export BUNDLE_FILENAME='$SOURCE_DIR/$BUNDLE_FILENAME'"
	echo "export CMAKE_PATH='$cmake_path'"
	echo "export CMAKE_EXEC='$cmake_path/bin/cmake.exe'"
) > settings.sh

for c in $compilers; do
	variants=compiler_${c}_variants
	for v in ${!variants}; do
		echo "Adding compiler $c variant $v"
		if [ ! -e compiler_config_${c}_${v}.sh ]; then
			$SOURCE_DIR/src/msbuild.sh "$compiler_v10_loader" $v $c || exit 1
		fi
	done
done

for c in $compilers; do
	variants=compiler_${c}_variants
	for v in ${!variants}; do
		for b in Release Debug; do
			echo "Build ACE, YARP, ICUB for $c $v $b"
			$SOURCE_DIR/src/build_ace.sh $c $v $b || exit 1
			$SOURCE_DIR/src/build_yarp.sh $c $v $b || exit 1
			$SOURCE_DIR/src/build_icub.sh $c $v $b || exit 1
		done
	done
done

