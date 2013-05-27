# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# This file is the official location of the current YARP version number.

## We use subversion to indentify current revision number (optional)
# the FindSubversion.cmake module is part of the standard distribution

# 29/8/12: Lorenzo Natale: Commenting out use of svn since it did not work with
# default installation of tortoise on Windows. Moving from YARP_SVN_REVISION to YARO_VERSION_TWEAK
# with similar usage.

# if(EXISTS ${CMAKE_SOURCE_DIR}/.svn)
    # find_package(Subversion)
    # if(Subversion_FOUND)
        # extract working copy information for SOURCE_DIR into YARP_XXX variables
        # Subversion_WC_INFO(${CMAKE_SOURCE_DIR} YARP)
    # endif(Subversion_FOUND)
# elseif(EXISTS ${CMAKE_SOURCE_DIR}/.git)
    # find_package(Git)
    # if(GIT_FOUND)
        # execute_process(COMMAND ${GIT_EXECUTABLE} svn info
                        # WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                        # OUTPUT_VARIABLE git_svn_info_output
                        # ERROR_VARIABLE git_svn_info_error
                        # RESULT_VARIABLE git_svn_info_result
                        # OUTPUT_STRIP_TRAILING_WHITESPACE)
        # if(NOT ${git_svn_info_result} EQUAL 0)
            # message(SEND_ERROR "Command \"${GIT_EXECUTABLE} svn info\" failed with output:\n${git_svn_info_error}")
        # else(NOT ${git_svn_info_result} EQUAL 0)
            # string(REGEX REPLACE "^(.*\n)?Revision: ([^\n]+).*"
                # "\\2" YARP_WC_REVISION "${git_svn_info_output}")
        # endif(NOT ${git_svn_info_result} EQUAL 0)
    # endif(GIT_FOUND)
# endif(EXISTS ${CMAKE_SOURCE_DIR}/.svn)

# if(YARP_WC_REVISION)
#   write a file with the SVNVERSION define
    # message(STATUS "Detected svn revision number: ${YARP_WC_REVISION}")
# endif(YARP_WC_REVISION)


set(YARP_VERSION_MAJOR "2")
set(YARP_VERSION_MINOR "3")
set(YARP_VERSION_PATCH "20")
set(YARP_VERSION_MODIFIER "")

## manually increase tweak number when required. Set to zero when
# bumping VERSION_PATCH
set(YARP_VERSION_TWEAK "4")
#"${YARP_WC_REVISION}")
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
