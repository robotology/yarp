# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
project(resourceFinder)

find_package(YARP COMPONENTS os REQUIRED)

add_executable(rf_basic)
target_sources(rf_basic PRIVATE tutorial_rf_basic.cpp)
target_link_libraries(rf_basic PRIVATE ${YARP_LIBRARIES})

add_executable(rf_advanced)
target_sources(rf_advanced PRIVATE tutorial_rf_advanced.cpp)
target_link_libraries(rf_advanced PRIVATE ${YARP_LIBRARIES})

# to install data files:
set(conf randomMotion/config.ini)
# alternatively, if you are too lazy to list all files explicitly, you can "GLOB" them with:
# file(GLOB conf conf/*.ini)

yarp_install(FILES ${conf} DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/randomMotion)
