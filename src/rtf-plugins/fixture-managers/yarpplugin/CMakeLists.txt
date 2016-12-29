# Copyright: (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.8.9)

# set the project name
set(PROJECTNAME YarpPluginFixture)
project(${PROJECTNAME})

# add the required cmake packages
find_package(RTF)
find_package(RTF COMPONENTS DLL)
find_package(YARP)

# add include directories
include_directories(${CMAKE_SOURCE_DIR}
                    ${RTF_INCLUDE_DIRS}
                    ${YARP_INCLUDE_DIRS}
                    ${YARP_HELPERS_INCLUDE_DIR})

# add required libraries 
link_libraries(${RTF_LIBRARIES}
               ${YARP_LIBRARIES})

# add the source codes to build the plugin library
add_library(${PROJECTNAME} MODULE YarpPluginFixture.h
                                  YarpPluginFixture.cpp)

# set the installation options
install(TARGETS ${PROJECTNAME}
        EXPORT ${PROJECTNAME}
        COMPONENT runtime
        LIBRARY DESTINATION lib)

