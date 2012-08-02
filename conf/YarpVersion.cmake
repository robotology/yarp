# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# This file is the official location of the current YARP version number.

## We use subversion to indentify current revision number (optional)
# the FindSubversion.cmake module is part of the standard distribution
find_package(Subversion)
if (Subversion_FOUND)
    # extract working copy information for SOURCE_DIR into YARP_XXX variables
    Subversion_WC_INFO(${CMAKE_SOURCE_DIR} YARP)
    # write a file with the SVNVERSION define
    message(STATUS "Detected svn revision number: ${YARP_WC_REVISION}")
endif()

set(YARP_VERSION_MAJOR "2")
set(YARP_VERSION_MINOR "3")
set(YARP_VERSION_PATCH "19")
set(YARP_VERSION_MODIFIER "")
set(YARP_SVN_REVISION "${YARP_WC_REVISION}")
set(YARP_VERSION_ABI "1")

set(CPACK_PACKAGE_VERSION_MAJOR "${YARP_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${YARP_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${YARP_VERSION_PATCH}${YARP_VERSION_MODIFIER}")

set(YARP_GENERIC_VERSION "${YARP_VERSION_MAJOR}.${YARP_VERSION_MINOR}.${YARP_VERSION_PATCH}")
set(YARP_GENERIC_SOVERSION "${YARP_VERSION_ABI}")
