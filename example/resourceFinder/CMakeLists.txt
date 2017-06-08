# Copyright: 2012 iCub Facility, Istituto Italiano di Tecnologia
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(resourceFinder)

find_package(YARP REQUIRED)

add_executable(rf_basic tutorial_rf_basic.cpp)
target_link_libraries(rf_basic ${YARP_LIBRARIES})

add_executable(rf_advanced tutorial_rf_advanced.cpp)
target_link_libraries(rf_advanced ${YARP_LIBRARIES})

# to install data files:
set(conf randomMotion/config.ini)
# alternatively, if you are too lazy to list all files explicitly, you can "GLOB" them with:
# file(GLOB conf conf/*.ini)

yarp_install(FILES ${conf} DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/randomMotion)

