CMAKE_MINIMUM_REQUIRED(VERSION 2.0.5)

PROJECT(YARP)

# set a flag so sub-directories know that are being compiled
# en masse as opposed to as individuals
SET(COMPILING_ALL_YARP TRUE)

# Pick up our scripts - they are all in the conf subdirectory
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/conf)

# Set up some large-scale compile options - do you want devices? guis?
INCLUDE(YarpOptions)

# Configure where the output will go
INCLUDE(YarpOutput)

# Detect the broad family of driver in use -- "linux" (actually unix) or 
# "winnt".  This is being phased out.
INCLUDE(YarpTag)

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
  ADD_CUSTOM_TARGET(dox COMMAND doxygen ${PROJECT_SOURCE_DIR}/conf/Doxyfile.part)
ENDIF (YARP_ADMIN)
