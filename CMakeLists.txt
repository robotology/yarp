CMAKE_MINIMUM_REQUIRED(VERSION 2.0.5)

PROJECT(YARP)

#I had to add this to make yarp compile on linux. Check if it
#is really needed. In case shoud move this to a better place --nat
SET(-D_REENTRANT)

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
