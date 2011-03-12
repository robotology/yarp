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
	echo -e "\t$0 $1\n"
) > $BUILD_DIR/Makefile

# load compiler settings
source $SOURCE_DIR/conf/compilers.sh || exit 1
if [ -e $SOURCE_DIR/conf/compilers_local.sh ]; then
	source $SOURCE_DIR/conf/compilers_local.sh || exit 1
fi

# export cmake_path=`cygpath --unix "$cmake_path"`

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
	loader=compiler_${c}_loader
	for v in ${!variants}; do
		echo "Adding compiler $c variant $v"
		if [ ! -e compiler_config_${c}_${v}.sh ]; then
			$SOURCE_DIR/src/msbuild.sh "${!loader}" $v $c || exit 1
		fi
	done
done

(
depend_cmake=
depend_ace=
depend_gsl=
depend_gtkmm=
depend_yarp="cmake ace gsl gtkmm"
depend_icub="cmake yarp ace gsl gtkmm"
depend_nsis=
depend_yarp_core_package="yarp nsis"
for t in cmake ace gsl gtkmm yarp icub nsis yarp_core_package; do
	depend_t=depend_$t
	deps=${!depend_t}
	targs=""
	for c in $compilers; do
		variants=compiler_${c}_variants
		for v in ${!variants}; do
			for b in Release Debug; do
				name="build_${t}_${c}_${v}_${b}.txt"
				fast_name="fast_${t}_${c}_${v}_${b}"
				targs="$targs $name"
				name_deps=""
				for d in $deps; do
					name_deps="$name_deps build_${d}_${c}_${v}_${b}.txt"
				done
				echo "$name:$name_deps"
				echo -e "\t$SOURCE_DIR/src/build_${t}.sh $c $v $b && touch $name\n"
				echo "$fast_name:"
				echo -e "\t$SOURCE_DIR/src/build_${t}.sh $c $v $b\n"
			done
		done
	done
	echo "$t.txt:$targs"
	echo -e "\ttouch $t.txt\n"
done
) >> $BUILD_DIR/Makefile

