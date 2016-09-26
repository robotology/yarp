# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

#
# Packing stuff
#

include(YarpVersion)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Yet Another Robot Platform")
set(CPACK_PACKAGE_NAME "yarp")
set(CPACK_PACKAGE_VENDOR "YARP")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_SOURCE_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

set(CPACK_PACKAGE_VERSION_MAJOR "${YARP_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${YARP_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${YARP_VERSION_PATCH}")
#set(CPACK_PACKAGE_VERSION_TWEAK "${YARP_VERSION_TWEAK}")
set(CPACK_PACKAGE_VERSION "${YARP_VERSION}")


#if you are making debian packages, this is useful, but not otherwise
#configure_file(${YARP_MODULE_DIR}/template/control.in
#               ${CMAKE_BINARY_DIR}/DEBIAN/control @ONLY)
#install_files(/doc/DEBIAN FILES ${CMAKE_BINARY_DIR}/DEBIAN/control)

if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
  set(CPACK_PACKAGE_EXECUTABLES "yarp" "yarp")
  include(CPack)
endif(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
