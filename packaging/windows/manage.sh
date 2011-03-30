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

# All basic targets to make available
full_target_list="cmake ace gsl gtkmm yarp nsis yarp_core_package"

# Coarse-grain dependencies between targets
depend_cmake=
depend_ace=
depend_gsl="cmake"
depend_gtkmm=
depend_yarp="cmake ace gsl gtkmm"
depend_icub="cmake yarp ace gsl gtkmm"
depend_nsis=
depend_yarp_core_package="yarp nsis"

# mark packages that don't have separate debug/release builds
build_out_cmake="any"
build_out_nsis="any"
build_out_yarp_core_package="any"

# mark packages that don't care about compiler
compilers_out_cmake="any"
compilers_out_nsis="any"
compiler_any_variants="any"

# fill in defaults
for t in $full_target_list; do
	build_out_t=build_out_$t
	build_out=${!build_out_t}
	build_out=${build_out:-Release Debug}
	export build_out_${t}="$build_out"
	compilers_out_t=compilers_out_$t
	compilers_out=${!compilers_out_t}
	compilers_out=${compilers_out:-$compilers}
	export compilers_out_${t}="$compilers_out"
done

declare -A TARGETS
declare -A DEP_TARGETS
function dep_target_list {
	local t=$1
	local c=$2
	local v=$3
	local b=$4
	local _build_out_t=build_out_$t
	_build_out="${!_build_out_t}"
	local _compilers_out_t=compilers_out_$t
	_compilers_out="${!_compilers_out_t}"
	if [ "k$_compilers_out" = "kany" ]; then
		c=any
		v=any
	else
		if [ "k$c" = "kany" ]; then
			c="$_compilers_out"
		fi
	fi
	if [ "k$_build_out" = "kany" ]; then
		b=any
	else
		if [ "k$b" = "kany" ]; then
			b="$_build_out"
		fi
	fi
	for c1 in $c; do
		for v1 in $v; do
			for b1 in $b; do
				target="build_${t}_${c1}_${v1}_${b1}.txt"
				DEP_TARGETS[$target]=1
			done
		done
	done
}

for t in $full_target_list; do
	depend_t=depend_$t
	deps=${!depend_t}
	build_out_t=build_out_$t
	build_out=${!build_out_t}
	compilers_out_t=compilers_out_$t
	compilers_out=${!compilers_out_t}
	TARGETS=()
	echo ""
	echo "# $t, build variants are ($build_out), compilers are ($compilers_out)"
	for c in $compilers_out; do
		variants=compiler_${c}_variants
		for v in ${!variants}; do
			for b in $build_out; do
				name="build_${t}_${c}_${v}_${b}.txt"
				fast_name="fast_${t}_${c}_${v}_${b}"
				TARGETS[$name]=1
				DEP_TARGETS=()
				for d in $deps; do
					dep_target_list $d $c $v $b
				done
				echo "$name: ${!DEP_TARGETS[*]}"
				echo -e "\t$SOURCE_DIR/src/build_${t}.sh $c $v $b && touch $name\n"
				#echo "$fast_name:"
				#echo -e "\t$SOURCE_DIR/src/build_${t}.sh $c $v $b\n"
			done
		done
	done
	#echo "build_${t}_any_any_any.txt: ${!TARGETS[*]}"
	#echo -e "\ttouch build_${t}_any_any_any.txt\n"
	echo "$t.txt: ${!TARGETS[*]}"
	echo -e "\ttouch $t.txt\n"
done
) >> $BUILD_DIR/Makefile

