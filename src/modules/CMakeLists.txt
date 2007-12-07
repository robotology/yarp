
PROJECT(YarpModules)

MESSAGE(STATUS "This is an experimental way to compile devices")

# This directory may be compiled independently of the rest of YARP,
# so we need to pick up YARP explicitly.
FIND_PACKAGE(YARP REQUIRED)

# Prepare a device library called "yarpmod".
# A test executable called "test_yarpmod" will also be produced.
BEGIN_DEVICE_LIBRARY(yarpmod)
ADD_SUBDIRECTORY(cuda)
ADD_SUBDIRECTORY(fakebot)
ADD_SUBDIRECTORY(nvidia)
ADD_SUBDIRECTORY(stage)
ADD_SUBDIRECTORY(vfw)
ADD_SUBDIRECTORY(wxsdl)
# we can include remote directories as well - we specify a 
# local directory for producing binaries so there is no interference
# with other possible builds.
ADD_SUBDIRECTORY(${YARP_DIR}/src/libYARP_dev/src/ffmpeg 
                 ${CMAKE_CURRENT_BINARY_DIR})
ADD_SUBDIRECTORY(${YARP_DIR}/src/libYARP_dev/src/microphone 
                 ${CMAKE_CURRENT_BINARY_DIR})
END_DEVICE_LIBRARY(yarpmod)

# Done!
#
# To use the library from CMake in your own projects, do:
#   FIND_PACKAGE(yarpmod)
#   LINK_LIBRARIES(yarpmod)
#
# To tell CMake about the location of this file, do:
#   SET(yarpmod_DIR /home/paulfitz/cvs/yarp2/src/modules)
# Or configure an equivalent environment variable, or enter the
# path in the CMake GUI.

