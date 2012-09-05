# Copyright: (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8)

set(BEND_OVER_BACKWARDS_COMPATIBLE TRUE)

message(STATUS "*** SWIG WRAPPERS HAVE MOVED, to \$YARP_ROOT/bindings ***")
include("${CMAKE_SOURCE_DIR}/../../bindings/CMakeLists.txt")
message(STATUS "*** SWIG WRAPPERS HAVE MOVED, to \$YARP_ROOT/bindings ***")
message(STATUS "*** Compilation in \$YARP_ROOT/example/swig will eventually stop working ***")

# all the juicy code that used to be here is now in $YARP_ROOT/bindings
