# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# This file is the official location of the current YARP version number.


## manually increase tweak number when required. Set to zero when
# bumping VERSION_PATCH
set(YARP_VERSION_MAJOR "2")
set(YARP_VERSION_MINOR "3")
set(YARP_VERSION_PATCH "67")
set(YARP_VERSION_TWEAK "3")
set(YARP_VERSION_STRING "")

set(YARP_VERSION_ABI "1")

# Generate YARP_VERSION
if(YARP_VERSION_TWEAK)
    set(YARP_VERSION "${YARP_VERSION_MAJOR}.${YARP_VERSION_MINOR}.${YARP_VERSION_PATCH}.${YARP_VERSION_TWEAK}")
else()
    set(YARP_VERSION "${YARP_VERSION_MAJOR}.${YARP_VERSION_MINOR}.${YARP_VERSION_PATCH}")
endif()

# Generate YARP_VERSION_STRING if not set
set(YARP_VERSION_STRING "${YARP_VERSION}")
if(NOT YARP_VERSION_STRING)
    set(YARP_VERSION_STRING "${YARP_VERSION}")
endif()

set(YARP_GENERIC_SOVERSION "${YARP_VERSION_ABI}")
