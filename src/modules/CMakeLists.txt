# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Giacomo Spigler
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

PROJECT(YarpModules)

# This directory may be compiled independently of the rest of YARP,
# so we need to pick up YARP explicitly.
IF (NOT COMPILING_ALL_YARP)
  FIND_PACKAGE(YARP REQUIRED)
ELSE (NOT COMPILING_ALL_YARP)
  INCLUDE(YarpDevice)
  SET (YARP_DIR ${CMAKE_SOURCE_DIR})
ENDIF (NOT COMPILING_ALL_YARP)


# Prepare a device library called "yarpmod".
# A test executable called "yarpmoddev" will also be produced.
BEGIN_DEVICE_LIBRARY(yarpmod)
  ADD_SUBDIRECTORY(cuda)
  ADD_SUBDIRECTORY(fakebot)
  ADD_SUBDIRECTORY(nvidia)
  ADD_SUBDIRECTORY(stage)
  ADD_SUBDIRECTORY(vfw)
  ADD_SUBDIRECTORY(wxsdl)
  ADD_SUBDIRECTORY(SerialServoBoard)
  ADD_SUBDIRECTORY(ffmpeg)
  ADD_SUBDIRECTORY(opencv)
  ADD_SUBDIRECTORY(microphone)
  ADD_SUBDIRECTORY(serial)
  ADD_SUBDIRECTORY(portaudio)
  ADD_SUBDIRECTORY(firewirecamera)
  ADD_SUBDIRECTORY(jrkerr)
  ADD_SUBDIRECTORY(urbtc)
  ADD_SUBDIRECTORY(dimax_u2c)

  # We can include remote directories as well - we specify a 
  # local directory for producing binaries so there is no interference
  # with other possible builds.
  # Actually, we don't do this anymore, but if we did, here's an
  # example of how to do it:
  # ADD_SUBDIRECTORY(${YARP_DIR}/src/libYARP_dev/src/dimax_u2c
  #                  ${CMAKE_CURRENT_BINARY_DIR}/dimax_u2c)

  # We can also suck in other device libraries built the same way.
  # We seek an ExternalModules.cmake file either in the conf directory
  # or in our build directory
  SET(EXTFILES "${YARP_MODULE_PATH}/ExternalModules.cmake"
    "${CMAKE_BINARY_DIR}/ExternalModules.cmake")
  FOREACH(EXTFILE ${EXTFILES})
    IF (EXISTS ${EXTFILE})
      INCLUDE(${EXTFILE})
      FOREACH(EXTDIR ${EXTERNAL_MODULES})
        IF (NOT ${EXTDIR}_DIR)
          SET(${EXTDIR}_DIR ${${EXTDIR}_PATH})
        ENDIF (NOT ${EXTDIR}_DIR)
	IF (NOT ${EXTDIR}_DIR)
	  MESSAGE(FATAL_ERROR "Need ${EXTDIR}_DIR in ${EXTFILE}")
	ENDIF (NOT ${EXTDIR}_DIR)
        # make sure path is usable - could have environment variable parts
        FILE(TO_CMAKE_PATH ${${EXTDIR}_DIR} safe_path)
	IF (NOT EXISTS "${safe_path}/CMakeLists.txt")
	  MESSAGE(FATAL_ERROR "${EXTDIR}_DIR=${${EXTDIR}_DIR} from ${EXTFILE} does not contain a CMakeLists.txt")
	ENDIF (NOT EXISTS "${safe_path}/CMakeLists.txt")
        ADD_SUBDIRECTORY(${safe_path}
                       ${CMAKE_CURRENT_BINARY_DIR}/externals)
      ENDFOREACH(EXTDIR ${EXTERNAL_MODULES})
    ENDIF (EXISTS ${EXTFILE})
  ENDFOREACH(EXTFILE)

END_DEVICE_LIBRARY(yarpmod)

IF(MSVC)
    SET_TARGET_PROPERTIES(yarpmod PROPERTIES COMPILE_FLAGS /wd4996)
ENDIF(MSVC)

INSTALL_TARGETS(/lib yarpmod)
FILE(GLOB header ${YARP_DIR}/*.h)
INSTALL_FILES(/include FILES ${header})


# Done!
#
# To use the library from CMake in your own projects, do:
#   FIND_PACKAGE(yarpmod)
#   LINK_LIBRARIES(yarpmod)
#
# To tell CMake about the location of this file, do:
#   SET(yarpmod_DIR ${YARP_DIR}/src/modules)
# Or configure an equivalent environment variable, or enter the
# path in the CMake GUI.

