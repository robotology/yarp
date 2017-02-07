#!/bin/bash

BUILD_DIR=$PWD

source ./settings.sh || {
  echo "No settings.sh found, are we in the build directory?"
  exit 1
}

source $SETTINGS_BUNDLE_FILENAME || {
  echo "Bundle settings not found"
  exit 1
}

source $SETTINGS_SOURCE_DIR/src/process_options.sh $* || {
  echo "Cannot process options"
  exit 1
}

source ace_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh || {
  echo "Cannot find corresponding ACE build"
  exit 1
}

# Removing GSL
#source gsl_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh || {
#  echo "Cannot find corresponding GSL build"
#  exit 1
#}
source gsl_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh || {
  echo "Cannot find corresponding EIGEN build"
  exit 1
}


source yarp_${OPT_COMPILER}_${OPT_VARIANT}_${OPT_BUILD}.sh || {
  echo "Cannot find corresponding YARP build"
  exit 1
}

if [ "k$BUNDLE_ICUB_VERSION" = "k" ]; then
  BUNDLE_ICUB_VERSION=trunk
fi

fname=icub-$BUNDLE_ICUB_VERSION

if [ ! -e $fname ]; then
  if [ "k$BUNDLE_ICUB_VERSION" = "ktrunk" ]; then
    svn co https://robotcub.svn.sourceforge.net/svnroot/robotcub/trunk/iCub/main $fname || {
      echo "Cannot fetch ICUB"
      exit 1
    }
  else
    echo "ICUB versioned check-out not set up yet"
    exit 1
  fi
fi

fname2=$fname-$OPT_COMPILER-$OPT_VARIANT-$OPT_BUILD

mkdir -p $fname2
cd $fname2 || exit 1

echo "Using ACE from $ACE_ROOT"
echo "Using YARP from $YARP_DIR"
  # Removing GSL
#"$CMAKE_EXEC" -DGSL_LIBRARY="$GSL_LIBRARY" -DGSLCBLAS_LIBRARY="$GSLCBLAS_LIBRARY" -DGSL_DIR="$GSL_DIR" -G "$OPT_GENERATOR" ../$fname || exit 1
"$CMAKE_EXEC" -G "$OPT_GENERATOR" ../$fname || exit 1
$OPT_BUILDER ICUB.sln $OPT_CONFIGURATION_COMMAND $OPT_PLATFORM_COMMAND
