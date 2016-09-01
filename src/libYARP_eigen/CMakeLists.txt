# Copyright: (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
# Author: Silvio Traversaro
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(YARP_eigen)

set(YARP_eigen_HDRS include/yarp/eigen/Eigen.h)

source_group("Header Files" FILES ${YARP_eigen_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")

# When we support CMake 3.0 as the minimum required, we can make this an INTERFACE library
# For now we just install the headers in a specific location
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/yarp/eigen DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp)