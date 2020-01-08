#!/bin/sh

# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

### parameters are
# $1: hostname (could be also buildtype)
# $2: os (macos, winxp, lenny, etch, karmic ...)
# $3: test type: nightly continuous experimental

CMAKE_OPTIONS="\
-DTEST_MACHINE_HOSTNAME:STRING=$1 \
-DTEST_MACHINE_OS_TYPE:STRING=$2 \
-DTEST_MACHINE_TEST_TYPE:STRING=$3 \
-DENABLE_DASHBOARD_SUBMIT:BOOL=TRUE \
-DCMAKE_SKIP_INSTALL_RPATH:BOOL=TRUE \
-DYARP_COMPILE_TESTS:BOOL=ON \
-DYARP_COMPILE_GUIS:BOOL=TRUE \
-DYARP_COMPILE_yarpviz:BOOL=TRUE \
-DYARP_COMPILE_libYARP_math:BOOL=TRUE \
-DENABLE_yarpcar_human:BOOL=TRUE \
-DENABLE_yarpcar_depthimage:BOOL=TRUE \
-DENABLE_yarpmod_fakebot:BOOL=TRUE \
-DENABLE_yarpmod_fakeMotionControl=TRUE \
-DENABLE_yarpmod_fakeAnalogSensor=TRUE \
-DENABLE_yarpmod_fakeIMU=TRUE \
-DENABLE_yarpmod_SerialServoBoard=TRUE \
-DENABLE_yarpmod_serialport=TRUE \
-DENABLE_yarpmod_imuBosch_BNO055=TRUE \
-DENABLE_yarpmod_fakeLaser=TRUE \
-DENABLE_yarpmod_rpLidar=TRUE \
-DENABLE_yarpmod_laserHokuyo=TRUE \
"
OS_TYPE=""
echo "$2" | grep -iq "Debian\|Ubuntu"
if [ "$?" == "0" ]; then
  OS_TYPE="linux"
  RELEASE_CODENAME=$(lsb_release -c | awk '{print $2}')
fi

echo "$2" | grep -iq "Windows"
if [ "$?" == "0" ]; then
  OS_TYPE="windows"
fi

echo "$2" | grep -iq "MacOSX"
if [ "$?" == "0" ]; then
  OS_TYPE="macosx"
fi

case $3 in
   "Experimental" )
      CMAKE_OPTIONS=" \
        $CMAKE_OPTIONS \
      "
      ;;
   "Continuous" )
     CMAKE_OPTIONS=" \
        $CMAKE_OPTIONS \
      "
      ;;
   "Nightly" )
      CMAKE_OPTIONS=" \
        $CMAKE_OPTIONS \
      "
      if [ "$OS_TYPE" == "linux" ]; then
        # On Debian 8 there are some issues with valgrind tests
        if [ "$RELEASE_CODENAME" != "jessie" ]; then
          CMAKE_OPTIONS=" \
            $CMAKE_OPTIONS \
            -DYARP_VALGRIND_TESTS=ON
          "
        fi
      fi
     ;;
esac
