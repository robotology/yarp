# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# This file is the official location of the current YARP version number.

## We use subversion to indentify current revision number (optional)
# the FindSubversion.cmake module is part of the standard distribution

# 29/8/12: Lorenzo Natale: Commenting out use of svn since it did not work with
# default installation of tortoise on Windows. Moving from YARP_SVN_REVISION to YARO_VERSION_TWEAK
# with similar usage.


set(YARP_VERSION_MAJOR "2")
set(YARP_VERSION_MINOR "3")
set(YARP_VERSION_PATCH "20")
set(YARP_VERSION_MODIFIER "")

## manually increase tweak number when required. Set to zero when
# bumping VERSION_PATCH
set(YARP_VERSION_TWEAK "5")
set(YARP_VERSION_ABI "1")

set(CPACK_PACKAGE_VERSION_MAJOR "${YARP_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${YARP_VERSION_MINOR}")
if(NOT YARP_VERSION_TWEAK)
    set(CPACK_PACKAGE_VERSION_PATCH "${YARP_VERSION_PATCH}${YARP_VERSION_MODIFIER}")
else(NOT YARP_VERSION_TWEAK)
    set(CPACK_PACKAGE_VERSION_PATCH "${YARP_VERSION_PATCH}.${YARP_VERSION_TWEAK}${YARP_VERSION_MODIFIER}")
endif(NOT YARP_VERSION_TWEAK)

set(YARP_GENERIC_VERSION "${YARP_VERSION_MAJOR}.${YARP_VERSION_MINOR}.${YARP_VERSION_PATCH}")
set(YARP_GENERIC_SOVERSION "${YARP_VERSION_ABI}")
