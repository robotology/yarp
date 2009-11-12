CMAKE_MINIMUM_REQUIRED(VERSION 2.0.5)

PROJECT(YARP)

# set some CMake policy flags (for CMake version 2.6)
IF (COMMAND cmake_policy)
  cmake_policy(SET CMP0005 OLD)  # use 2.4 behavior for ADD_DEFINITIONS
  cmake_policy(SET CMP0003 NEW)  # don't do funky lib path things
ENDIF (COMMAND cmake_policy)

# set a flag so sub-directories know that are being compiled
# en masse as opposed to as individuals
SET(COMPILING_ALL_YARP TRUE)

# Pick up our scripts - they are all in the conf subdirectory
SET(YARP_MODULE_PATH ${PROJECT_SOURCE_DIR}/conf)
SET(CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

# Set up some large-scale compile options - do you want devices? guis?
INCLUDE(YarpOptions)

# Get the current yarp version.  Really this should be placed into
# a generated header file.
INCLUDE(YarpVersion)

# Configure where the output will go
INCLUDE(YarpOutput)

# get appropriate libraries and settings
INCLUDE(YarpReqLib)

# define useful macros for sub-libraries and programs
INCLUDE(YarpLibrary)
INCLUDE(YarpProgram)

# turn on testing
ENABLE_TESTING()

# declare active subdirectories
SUBDIRS(src conf)

# set up some packaging properties
INCLUDE(YarpPackage)

# make doc target - we could check whether doxygen is present, but
# let's not make a big deal of it for now.
SET(YARP_ADMIN "$ENV{YARP_ADMIN}")
IF (YARP_ADMIN)
  ADD_CUSTOM_TARGET(dox COMMAND doxygen ${PROJECT_SOURCE_DIR}/conf/doxygen/Doxyfile.part)
ENDIF (YARP_ADMIN)

# Modify this file when you add/remove files in directories that
# are scanned for sources.  This will make sure that cmake gets rerun
# for everybody.
# Touch: 1 2 3
