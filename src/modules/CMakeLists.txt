# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Giacomo Spigler
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
# Thanks to Rui Silva for a sugggested improvement to external module support

option(CREATE_DEVICE_LIBRARY_MODULES "Compile device modules" FALSE)
if (CREATE_DEVICE_LIBRARY_MODULES)

project(YarpModules)

include(YarpPlugin)

# Prepare a plugin library called "yarpmod".
YARP_BEGIN_PLUGIN_LIBRARY(yarpmod)
  add_subdirectory(cuda)
  add_subdirectory(fakebot)
  add_subdirectory(fakeMotionControl)
  add_subdirectory(fakeIMU)
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
  add_subdirectory(imuBosch_BNO055)
  add_subdirectory(jrkerr)
  add_subdirectory(urbtc)
  add_subdirectory(DynamixelAX12Ftdi)
  add_subdirectory(fakeLaser)
  add_subdirectory(rpLidar)
  add_subdirectory(laserHokuyo)
  add_subdirectory(kinect)
  add_subdirectory(openni2)
  add_subdirectory(ovrheadset)
  add_subdirectory(PumaCalibrator)
  add_subdirectory(TransformClient)

  # The dimax_u2c/blob is not exported in the tarball, therefore we
  # disable the build if the folder does not exist
  if(EXISTS ${CMAKE_CURRENT_DIRECTORY}/dimax_u2c/blob)
      add_subdirectory(dimax_u2c)
  endif()

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
  set(EXTFILES "${YARP_MODULE_DIR}/ExternalModules.cmake"
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
                         ${CMAKE_CURRENT_BINARY_DIR}/externals/${EXTDIR})
      endforeach(EXTDIR ${EXTERNAL_MODULES})
    endif (EXISTS ${EXTFILE})
  endforeach(EXTFILE)

YARP_END_PLUGIN_LIBRARY(yarpmod)

install(TARGETS yarpmod
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

if (YARP_LINK_PLUGINS)
  set_property(GLOBAL APPEND PROPERTY YARP_LIBS yarpmod ${yarpmod_LIBRARIES})
endif()

endif (CREATE_DEVICE_LIBRARY_MODULES)
