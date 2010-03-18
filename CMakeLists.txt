# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.6)

project(YARP)

# set a flag so sub-directories know that are being compiled
# en masse as opposed to as individuals
set(COMPILING_ALL_YARP TRUE)

# Pick up our scripts - they are all in the conf subdirectory
set(YARP_MODULE_PATH ${PROJECT_SOURCE_DIR}/conf)
set(CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

# Set up some large-scale compile options - do you want devices? guis?
include(YarpOptions)

# Get the current yarp version.  Really this should be placed into
# a generated header file.
include(YarpVersion)

# Configure where the output will go
include(YarpOutput)

# get appropriate libraries and settings
include(YarpReqLib)

# define useful macros for sub-libraries and programs
#INCLUDE(YarpLibrary)
#INCLUDE(YarpProgram)

# turn on testing
enable_testing()

set_property(GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
set_property(GLOBAL PROPERTY YARP_TREE_LIBRARIES)

# declare active subdirectories
add_subdirectory(conf)
ADD_SUBDIRECTORY(src)

get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
message(STATUS "Include dirs ${YARP_TREE_INCLUDE_DIRS}")

# set up some packaging properties
include(YarpPackage)

# make doc target - we could check whether doxygen is present, but
# let's not make a big deal of it for now.
set(YARP_ADMIN "$ENV{YARP_ADMIN}")
if (YARP_ADMIN)
  configure_file(${CMAKE_SOURCE_DIR}/conf/doxygen/Doxyfile.part.template
                 ${CMAKE_BINARY_DIR}/dox/Doxyfile.part IMMEDIATE)
  add_custom_target(dox COMMAND doxygen ${CMAKE_BINARY_DIR}/dox/Doxyfile.part)
endif ()

# Modify this file when you add/remove files in directories that
# are scanned for sources.  This will make sure that cmake gets rerun
# for everybody.
# Touch: 1 2 3
