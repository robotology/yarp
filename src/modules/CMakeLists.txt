
PROJECT(YarpModules)

MESSAGE(STATUS "This is an experimental way to compile devices")

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
  # we can include remote directories as well - we specify a 
  # local directory for producing binaries so there is no interference
  # with other possible builds.
  ADD_SUBDIRECTORY(${YARP_DIR}/src/libYARP_dev/src/serial 
                   ${CMAKE_CURRENT_BINARY_DIR}/serial)
  ADD_SUBDIRECTORY(${YARP_DIR}/src/libYARP_dev/src/firewirecamera
                   ${CMAKE_CURRENT_BINARY_DIR}/firewirecamera)
  ADD_SUBDIRECTORY(${YARP_DIR}/src/libYARP_dev/src/portaudio
                   ${CMAKE_CURRENT_BINARY_DIR}/portaudio)
  ADD_SUBDIRECTORY(${YARP_DIR}/src/libYARP_dev/src/urbtc
                   ${CMAKE_CURRENT_BINARY_DIR}/urbtc)
  ADD_SUBDIRECTORY(${YARP_DIR}/src/libYARP_dev/src/jrkerr
                   ${CMAKE_CURRENT_BINARY_DIR}/jrkerr)
  ADD_SUBDIRECTORY(${YARP_DIR}/src/libYARP_dev/src/dimax_u2c
                   ${CMAKE_CURRENT_BINARY_DIR}/dimax_u2c)

  # we can also suck in other device libraries built the same way
  SET(EXTFILE "${YARP_MODULE_PATH}/ExternalModules.cmake")
  IF (EXISTS ${EXTFILE})
    INCLUDE(${EXTFILE})
    FOREACH(EXTDIR ${EXTERNAL_MODULES})
      IF (NOT ${EXTDIR}_DIR)
        SET(${EXTDIR}_DIR ${${EXTDIR}_PATH})
      ENDIF (NOT ${EXTDIR}_DIR)
      ADD_SUBDIRECTORY(${${EXTDIR}_DIR}
                     ${CMAKE_CURRENT_BINARY_DIR}/externals)
    ENDFOREACH(EXTDIR ${EXTERNAL_MODULES})
  ENDIF (EXISTS ${EXTFILE})

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

