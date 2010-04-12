# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Giacomo Spigler
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

project(YarpModules)

include(YarpDevice)
if (NOT YARP_DIR)
  set (YARP_DIR ${CMAKE_SOURCE_DIR})
endif (NOT YARP_DIR)

# Prepare a device library called "yarpmod".
# A test executable called "yarpmoddev" will also be produced.
BEGIN_DEVICE_LIBRARY(yarpmod)
  add_subdirectory(cuda)
  add_subdirectory(fakebot)
  add_subdirectory(nvidia)
  add_subdirectory(stage)
  add_subdirectory(vfw)
  add_subdirectory(wxsdl)
  add_subdirectory(SerialServoBoard)
  add_subdirectory(ffmpeg)
  add_subdirectory(opencv)
  add_subdirectory(microphone)
  add_subdirectory(serial)
  add_subdirectory(portaudio)
  add_subdirectory(firewirecamera)
  add_subdirectory(jrkerr)
  add_subdirectory(urbtc)
  add_subdirectory(dimax_u2c)

  # We can include remote directories as well - we specify a 
  # local directory for producing binaries so there is no interference
  # with other possible builds.
  # Actually, we don't do this anymore, but if we did, here's an
  # example of how to do it:
  # add_subdirectory(${YARP_DIR}/src/libYARP_dev/src/dimax_u2c
  #                  ${CMAKE_CURRENT_BINARY_DIR}/dimax_u2c)

  # We can also suck in other device libraries built the same way.
  # We seek an ExternalModules.cmake file either in the conf directory
  # or in our build directory
  set(EXTFILES "${YARP_MODULE_PATH}/ExternalModules.cmake"
    "${CMAKE_BINARY_DIR}/ExternalModules.cmake")
  foreach(EXTFILE ${EXTFILES})
    if (EXISTS ${EXTFILE})
      include(${EXTFILE})
      foreach(EXTDIR ${EXTERNAL_MODULES})
        if (NOT ${EXTDIR}_DIR)
          set(${EXTDIR}_DIR ${${EXTDIR}_PATH})
        endif (NOT ${EXTDIR}_DIR)
	if (NOT ${EXTDIR}_DIR)
	  message(FATAL_ERROR "Need ${EXTDIR}_DIR in ${EXTFILE}")
	endif (NOT ${EXTDIR}_DIR)
        # make sure path is usable - could have environment variable parts
        file(TO_CMAKE_PATH ${${EXTDIR}_DIR} safe_path)
	if (NOT EXISTS "${safe_path}/CMakeLists.txt")
	  message(FATAL_ERROR "${EXTDIR}_DIR=${${EXTDIR}_DIR} from ${EXTFILE} does not contain a CMakeLists.txt")
	endif (NOT EXISTS "${safe_path}/CMakeLists.txt")
        add_subdirectory(${safe_path}
                       ${CMAKE_CURRENT_BINARY_DIR}/externals)
      endforeach(EXTDIR ${EXTERNAL_MODULES})
    endif (EXISTS ${EXTFILE})
  endforeach(EXTFILE)

END_DEVICE_LIBRARY(yarpmod)

if(MSVC)
    set_target_properties(yarpmod PROPERTIES COMPILE_FLAGS /wd4996)
endif(MSVC)

install(TARGETS yarpmod COMPONENT devices DESTINATION lib)
file(GLOB header ${YARP_DIR}/*.h)
install_files(/include FILES ${header})


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

