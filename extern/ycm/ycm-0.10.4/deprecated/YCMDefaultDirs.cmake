#.rst:
# YCMDefaultDirs
# --------------

#=============================================================================
# Copyright 2013 Istituto Italiano di Tecnologia (IIT)
#   Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

include(${CMAKE_CURRENT_LIST_DIR}/YCMDeprecatedWarning.cmake)
ycm_deprecated_warning("YCMDefaultDirs.cmake is deprecated.")

include(GNUInstallDirs)

macro(YCM_DEFAULT_DIRS _prefix)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

    set(${_prefix}_BUILD_LIBDIR ${CMAKE_INSTALL_LIBDIR})
    set(${_prefix}_BUILD_BINDIR ${CMAKE_INSTALL_BINDIR})
    set(${_prefix}_BUILD_INCLUDEDIR ${CMAKE_SOURCE_DIR}/src)

    set(${_prefix}_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
    set(${_prefix}_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR})
    set(${_prefix}_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
endmacro()
