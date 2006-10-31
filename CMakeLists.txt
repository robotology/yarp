CMAKE_MINIMUM_REQUIRED(VERSION 2.0.5)

PROJECT(YARP)

##################################################
#ask user to specify build type (linux only)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING 
	"Choose the type of build, recommanded options are: Debug or Release")
ENDIF(NOT CMAKE_BUILD_TYPE)
# hide variable to WINDOWS users (CMAKE_BUILD_TYPE is not used on win)
IF (WIN32 AND NOT CYGWIN)
  MARK_AS_ADVANCED(CMAKE_BUILD_TYPE)
ENDIF(WIN32 AND NOT CYGWIN)
#################################################

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
