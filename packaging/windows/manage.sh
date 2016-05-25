#!/bin/bash
#set -x
##############################################################################
#
# Copyright: (C) 2011 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
# This script configures a build directory.  To use it, do something like:
#   mkdir build
#   cd build
#   $SOURCE_DIR/manage.sh
# where $SOURCE_DIR is the path to this script.
#
# This will list a set of "bundles" that can be built.  Each named
# bundle $BUNDLE corresponds to specific versions of software packages 
# used to put together a named binary release (e.g. bundle-2-3-3 puts 
# YARP 2.3.3 together with CMake 2.8.4, GSL 1.14, etc).
#
# Each bundle $BUNDLE corresponds to a file named:
#   $SOURCE_DIR/conf/bundle-$BUNDLE.sh
# To see the versions of software involved in a bundle, read that file.
# If no bundle file exists that does what you want, make a new one.
# Then, in your build directory $BUILD_DIR, do:
#   $SOURCE_DIR/manage.sh $BUNDLE
# This will work for a while, and then produce at least the following files:
#   $BUILD_DIR/Makefile     # targets for building release
#   $BUILD_DIR/settings.sh  # cache of build variables, including $BUNDLE
#


# For simplicity, make sure this script is not called in-place
if [ -e manage.sh ]; then
  echo "Please call from a build directory"
  exit 1
fi

# Check bash isn't too old to support associative arrays
declare -A check_for_associative_arrays || (
  echo "Update bash version"
  exit 1
)

# Set up build and source directory
BUILD_DIR=$PWD
relative_dir="`dirname $0`"
cd "$relative_dir"
SOURCE_DIR=$PWD
cd $BUILD_DIR

# If no argument given, report available "bundles" (lists of software 
# package versions) and exit.  We generate some temporary files to
# enable tab completion.
BUNDLES="`cd $SOURCE_DIR/conf; ls -1 bundle*.sh | sed "s/\.sh//"`"
if [ "k$1" = "k" ]; then
  echo "Please specify a bundle name.  One of these:"
  # Support tab completion
  for b in $BUNDLES; do
      echo -n " $b"
      echo "$0 $b" > $b
      chmod u+x $b
  done
  echo " "
  exit 1
else
  # Remove tab completion
  for b in $BUNDLES; do
      rm -f $b
  done
fi

# Load bundle settings
BUNDLE_NAME="$1"
BUNDLE_FILENAME="$SOURCE_DIR/conf/$1.sh"
if [ ! -e "$BUNDLE_FILENAME" ]; then
  echo "Cannot find $BUNDLE_FILENAME"
  exit 1
fi
source $BUNDLE_FILENAME

# Start preparing Makefile.  The default target is to rerun
# this script with the desired bundle.
MAKEFILE=$BUILD_DIR/Makefile
(
  echo "# Default target is to simply rerun the command used to generate this Makefile"
  echo "default:"
  echo -e "\t$0 $1\n"
) > $MAKEFILE
creation_list="Makefile"

# Load default list of compilers
source $SOURCE_DIR/conf/compilers.sh || exit 1

# Override list of compilers with local configuration, if available
if [ -e $SOURCE_DIR/conf/compilers_local.sh ]; then
  source $SOURCE_DIR/conf/compilers_local.sh || exit 1
fi

# Cache important variables in $BUILD_DIR/settings.sh
(
  echo "export nodosfilewarning=1 # inhibit cygwin warning about f:/foo/bar"
  echo "export SETTINGS_SOURCE_DIR='$SOURCE_DIR'"
  echo "export SETTINGS_BUNDLE_NAME='$BUNDLE_NAME'"
  echo "export SETTINGS_BUNDLE_FILENAME='$BUNDLE_FILENAME'"
) > $BUILD_DIR/settings.sh
creation_list="$creation_list, settings.sh"

# Scan each compiler in list of compilers, and all desired variants.
# This gives an opportunity to cache the settings needed for 
# individual compilers.  At least for MSVC, this speeds things up
# a lot.  Compiler setting caches are written to:
#   $BUILD_DIR/compiler_config_${COMPILER_NAME}_${VARIANT_NAME}.sh
echo "Scanning compilers..."
for c in $compilers; do
  variants=compiler_${c}_variants
  loader_t=compiler_${c}_loader
  family_t=compiler_${c}_family
  loader="${!loader_t}"
  family="${!family_t}"
  for v in ${!variants}; do
    echo "Adding compiler $c variant $v family $family"
    if [ ! -e compiler_config_${c}_${v}.sh ]; then
      $SOURCE_DIR/src/load_$family.sh "$loader" $v $c || {
          echo " "
          echo "Failed to scan compiler $c variant $v (family $family)"
          echo "Please check the settings in:"
          echo "  $SOURCE_DIR/conf/compilers.sh"
          echo "  $SOURCE_DIR/conf/compilers_local.sh"
          exit 1
      }
      creation_list="$creation_list, compiler_config_${c}_${v}.sh"
    fi
  done
done


# Okay, the administrivia is finished.  Now, we work on creating
# make targets for the various software packages

# Here's the list of all available targets we care about
full_target_list="cmake ace gsl gtkmm qt yarp nsis yarp_core_package"

# For package $PACKAGE, depend_$PACKAGE contains a list of packages
# it depends on
depend_cmake=
depend_ace=
depend_gsl="cmake"
depend_gtkmm=
depend_qt=
depend_yarp="cmake ace gsl gtkmm qt"
depend_icub="cmake yarp ace gsl gtkmm"
depend_nsis=
depend_yarp_core_package="yarp nsis"

# We mark packages that don't have separate debug/release builds by
# setting build_out_$PACKAGE to "any"
build_out_cmake="any"
build_out_nsis="any"
build_out_yarp_core_package="any"

# We mark packages that don't need a compiler by setting
# compilers_out_$PACKAGE to "any"
compilers_out_cmake="any"
compilers_out_nsis="any"

# This variable simplifies later scripts, saying that a package
# with compilers_out_$PACKAGE set to "any" also doesn't care
# about compiler variants
compiler_any_variants="any"

# For $build_out_$PACKAGE and $compilers_out_$PACKAGE variables 
# that are not set, we provide defaults, to simplify scripting
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

# Global list of targets
declare -A TARGETS

# Temporary list of dependencies for a given target
declare -A DEP_TARGETS

# Function to compute the dependencies of a named target.  Call as:
#   dep_target_list $TARGET $COMPILER $VARIANT $BUILD
# Result is given in keys of $DEP_TARGETS
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

# Scan through the set of targets, and generate appropriate make commands
# to build them
for t in $full_target_list; do
  depend_t=depend_$t
  deps=${!depend_t}
  build_out_t=build_out_$t
  build_out=${!build_out_t}
  compilers_out_t=compilers_out_$t
  compilers_out=${!compilers_out_t}
  TARGETS=()
  echo "" >> $MAKEFILE
  echo "# $t, build variants are ($build_out), compilers are ($compilers_out)" >> $MAKEFILE
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
        echo "$name: ${!DEP_TARGETS[*]}" >> $MAKEFILE
        echo -e "\t$SOURCE_DIR/src/build_${t}.sh $c $v $b && touch $name\n" >> $MAKEFILE
      done
    done
  done
  echo "$t.txt: ${!TARGETS[*]}" >> $MAKEFILE
  echo -e "\ttouch $t.txt\n" >> $MAKEFILE
done

# Tell the user we're done, and the targets available.
echo " "
echo "Created: $creation_list"
echo "Here are the main targets available:"
for t in $full_target_list; do
    echo "  make $t.txt"
done
echo "For more fine-grained targets, please read the Makefile."
echo "Delete a *.txt file if you want to rebuild the corresponding target."
