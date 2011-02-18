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

cd $BUILD_DIR
(
	echo "export SOURCE_DIR='$SOURCE_DIR'"
	echo "export BUNDLE_NAME='$BUNDLE_NAME'"
	echo "export BUNDLE_FILENAME='$SOURCE_DIR/$BUNDLE_FILENAME'"
) > settings.sh

# load compiler settings
source $SOURCE_DIR/conf/compilers.sh || exit 1
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
		for b in Debug Release; do
			echo "Build ACE for $c $v $b"
			$SOURCE_DIR/src/build_ace.sh $c $v $b || exit 1
		done
	done
done

