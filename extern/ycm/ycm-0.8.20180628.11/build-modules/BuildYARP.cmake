#.rst:
# BuildYARP
# ---------
#
# YARP

#=============================================================================
# Copyright 2013-2014 Istituto Italiano di Tecnologia (IIT)
#   Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)

include(YCMEPHelper)
include(FindOrBuildPackage)


find_or_build_package(TinyXML QUIET)
if(COMMAND set_package_properties)
    set_package_properties(TinyXML PROPERTIES PURPOSE "Used by YARP")
endif()

find_or_build_package(GooCanvasMM QUIET)
if(COMMAND set_package_properties)
    set_package_properties(GooCanvasMM PROPERTIES PURPOSE "Used by YARP")
endif()

find_or_build_package(GtkDataboxMM QUIET)
if(COMMAND set_package_properties)
    set_package_properties(GtkDatabox PROPERTIES PURPOSE "Used by YARP")
endif()

# find_or_build_package(SQLite)


# For bindings
find_package(SWIG QUIET)
find_package(PythonLibs QUIET)
find_package(PythonInterp QUIET)

ycm_ep_helper(YARP TYPE GIT
                   STYLE GITHUB
                   REPOSITORY robotology/yarp.git
                   TAG master
                   DEPENDS TinyXML
                           GooCanvasMM
                           GtkDataboxMM
                   CMAKE_CACHE_ARGS -DCREATE_IDLS:BOOL=ON
                                    -DCREATE_GUIS:BOOL=ON
                                    -DCREATE_SHARED_LIBRARY:BOOL=ON
                                    -DYARP_USE_SYSTEM_SQLITE:BOOL=ON
                                    -DCREATE_LIB_MATH:BOOL=ON
                                    -DYARP_USE_ATLAS:BOOL=OFF
                                    -DDOX_GENERATE_XML:BOOL=ON
                                    -DCREATE_OPTIONAL_CARRIERS:BOOL=ON
                                    -DENABLE_yarpcar_bayer_carrier:BOOL=ON
                                    -DUSE_LIBDC1394:BOOL=OFF
                                    -DENABLE_yarpcar_tcpros_carrier:BOOL=ON
                                    -DENABLE_yarpcar_xmlrpc_carrier:BOOL=ON
                                    -DENABLE_yarpcar_priority_carrier:BOOL=ON
                                    -DENABLE_yarpcar_bayer_carrier:BOOL=ON
                                    -DENABLE_yarpidl_thrift:BOOL=ON
                                    -DCREATE_DEVICE_LIBRARY_MODULES:BOOL=ON
                                    -DENABLE_yarpcar_human_carrier:BOOL=ON
                                    -DENABLE_yarpcar_mjpeg_carrier:BOOL=ON
                                    -DENABLE_yarpcar_rossrv_carrier:BOOL=ON
                                    -DENABLE_yarpmod_fakebot:BOOL=ON
                                    -DENABLE_yarpmod_opencv_grabber:BOOL=OFF
                                    -DYARP_CLEAN_API:BOOL=ON
                                    -DYARP_COMPILE_TESTS:BOOL=OFF
                                    -DCREATE_YARPBUILDER:BOOL=ON
                                    -DYARP_COMPILE_EXPERIMENTAL_WRAPPERS:BOOL=ON
                                    -DYARP_DOXYGEN_XML:BOOL=ON
                                    -DYARP_DOXYGEN_TAGFILE:BOOL=ON
                                    -DYARP_COMPILE_BINDINGS:BOOL=ON
                                    -DCREATE_PYTHON:BOOL=ON
                                    -DYARP_COMPILE_TESTS:BOOL=ON
                   TEST_BEFORE_INSTALL 1
                   TEST_EXCLUDE_FROM_MAIN 1)
