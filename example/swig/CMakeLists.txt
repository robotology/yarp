# Copyright: (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

set(BEND_OVER_BACKWARDS_COMPATIBLE TRUE)

set(MSG "*** Read README.txt - SWIG WRAPPERS HAVE MOVED, to \$YARP_ROOT/bindings ***")
set(A_MSG_0 ${MSG} CACHE STRING ${MSG})
set(A_MSG_1 ${MSG} CACHE STRING ${MSG})
set(A_MSG_2 ${MSG} CACHE STRING ${MSG})
set(A_MSG_3 ${MSG} CACHE STRING ${MSG})
set(A_MSG_4 ${MSG} CACHE STRING ${MSG})

message(STATUS ${MSG})
message(STATUS ${MSG})
message(STATUS ${MSG})
message(STATUS ${MSG})

set(BINDINGS_SRC_DIR ${CMAKE_SOURCE_DIR}/../../bindings/src)
# Have to make copy of yarp.i and some other files
configure_file(${CMAKE_SOURCE_DIR}/../../bindings/yarp.i yarp.i)

include("${CMAKE_SOURCE_DIR}/../../bindings/CMakeLists.txt")

message(STATUS ${MSG})
message(STATUS ${MSG})
message(STATUS ${MSG})
message(STATUS ${MSG})

# all the juicy code that used to be here is now in $YARP_ROOT/bindings
