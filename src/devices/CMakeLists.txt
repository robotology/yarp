# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

include(YarpPlugin)
include(YarpPrintFeature)

yarp_begin_plugin_library(yarpmod
  OPTION YARP_COMPILE_DEVICE_PLUGINS
  DEFAULT ON
)

  add_subdirectory(test)
  add_subdirectory(fake)
  add_subdirectory(hardware)
  add_subdirectory(nws_nwc_yarp)
  add_subdirectory(nws_nwc_ros)

  # We can also suck in other device libraries built the same way.
  # We seek an ExternalModules.cmake file either in the conf directory
  # or in our build directory
  set(EXTFILES
    "${YARP_MODULE_DIR}/ExternalModules.cmake"
    "${CMAKE_BINARY_DIR}/ExternalModules.cmake"
  )
  foreach(EXTFILE ${EXTFILES})
    if(EXISTS ${EXTFILE})
      include(${EXTFILE})
      foreach(EXTDIR ${EXTERNAL_MODULES})
        if(NOT ${EXTDIR}_DIR)
          set(${EXTDIR}_DIR ${${EXTDIR}_PATH})
        endif()
        if(NOT ${EXTDIR}_DIR)
          message(FATAL_ERROR "Need ${EXTDIR}_DIR in ${EXTFILE}")
        endif()
        # make sure path is usable - could have environment variable parts
        file(TO_CMAKE_PATH ${${EXTDIR}_DIR} safe_path)
        if(NOT EXISTS "${safe_path}/CMakeLists.txt")
          message(FATAL_ERROR "${EXTDIR}_DIR=${${EXTDIR}_DIR} from ${EXTFILE} does not contain a CMakeLists.txt")
        endif()
        add_subdirectory(${safe_path}
                         ${CMAKE_CURRENT_BINARY_DIR}/externals/${EXTDIR})
      endforeach()
    endif()
  endforeach()

yarp_end_plugin_library(yarpmod QUIET)
add_library(YARP::yarpmod ALIAS yarpmod)


install(
  TARGETS yarpmod
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_yarpmod
  DEPENDENCIES ${YARP_yarpmod_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_yarpmod_PRIVATE_DEPS}
)

set_property(TARGET yarpmod PROPERTY FOLDER "Plugins/Device")

yarp_renamed_option(CREATE_DEVICE_LIBRARY_MODULES YARP_COMPILE_DEVICE_PLUGINS) # since YARP 3.0.0
