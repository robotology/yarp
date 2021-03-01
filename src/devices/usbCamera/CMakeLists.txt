# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

include(FetchContent)

option(COMPILE_WITHUNITTEST_ULTRAPYTHON "Compile also unittest for UltraPython cam board" OFF)

# Get rid of undef while compiling gtest -- START
get_property(
    compile_options
    DIRECTORY
    PROPERTY COMPILE_OPTIONS
)
set_property(
    DIRECTORY
    APPEND
    PROPERTY COMPILE_OPTIONS -Wno-undef -Wno-error
)


# Get rid of undef while compiling gtest -- END

if(COMPILE_WITHUNITTEST_ULTRAPYTHON)
  FetchContent_Declare(
    unittest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.10.0
  )
  FetchContent_GetProperties(unittest)
  if(NOT unittest)
    FetchContent_Populate(unittest)
    set(BUILD_SHARED_LIBS_SAVED "${BUILD_SHARED_LIBS}")
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(unittest)
    set(BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS_SAVED}")
    add_subdirectory(${unittest_SOURCE_DIR} ${unittest_BINARY_DIR})
  endif()
endif()

# Get rid of undef while compiling gtest -- START
set_property(
    DIRECTORY
    PROPERTY COMPILE_OPTIONS ${compile_options}
)
unset(compile_options)

# Get rid of undef while compiling gtest -- END

yarp_prepare_plugin(usbCamera
                    CATEGORY device
                    TYPE USBCameraDriverRgb
                    INCLUDE common/USBcamera.h
                    EXTRA_CONFIG WRAPPER=grabberDual
                    DEPENDS "UNIX;NOT APPLE;YARP_HAS_Libv4l2;YARP_HAS_Libv4lconvert;YARP_HAS_OpenCV;YARP_HAS_LibUDev")

yarp_prepare_plugin(usbCameraRaw
                    CATEGORY device
                    TYPE USBCameraDriverRaw
                    INCLUDE common/USBcamera.h
                    EXTRA_CONFIG WRAPPER=grabberDual
                    DEPENDS "UNIX;NOT APPLE;YARP_HAS_Libv4l2;YARP_HAS_Libv4lconvert;YARP_HAS_OpenCV;YARP_HAS_LibUDev")

if(ENABLE_usbCamera OR ENABLE_usbCameraRaw)
  yarp_add_plugin(yarp_usbCamera)

  target_sources(yarp_usbCamera PRIVATE common/USBcamera.cpp
                                        common/USBcamera.h
                                        common/USBcameraLogComponent.cpp
                                        common/USBcameraLogComponent.h
                                        linux/V4L_camera.cpp
                                        linux/V4L_camera.h
                                        linux/list.cpp
                                        linux/list.h
                                        linux/PythonCameraHelper.cpp
                                        linux/PythonCameraHelper.h
                                        linux/xilinx-v4l2-controls.h
                                        linux/InterfaceForCFunction.h
                                        )

  target_include_directories(yarp_usbCamera PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/common")
  target_include_directories(yarp_usbCamera PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/linux")

  target_link_libraries(yarp_usbCamera PRIVATE YARP::YARP_os
                                               YARP::YARP_sig
                                               YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  target_include_directories(yarp_usbCamera PRIVATE ${Libv4l2_INCLUDE_DIRS})
  target_link_libraries(yarp_usbCamera PRIVATE ${Libv4l2_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS Libv4l2) (not using targets)

  target_include_directories(yarp_usbCamera PRIVATE ${Libv4lconvert_INCLUDE_DIRS})
  target_link_libraries(yarp_usbCamera PRIVATE ${Libv4lconvert_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS Libv4lconvert) (not using targets)

  target_include_directories(yarp_usbCamera SYSTEM PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(yarp_usbCamera PRIVATE ${OpenCV_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS OpenCV) (not using targets)

  target_include_directories(yarp_usbCamera SYSTEM PRIVATE ${UDev_INCLUDE_DIRS})
  target_link_libraries(yarp_usbCamera PRIVATE ${UDev_LIBRARIES})
#   list(APPEND YARP_${​YARP_PLUGIN_MASTER}​_PRIVATE_DEPS UDev) (not using targets until FindUDev.cmake is in YCM)

  #target_link_libraries(yarp_usbCamera PRIVATE udev) #only for ARM PYTHON

  yarp_install(TARGETS yarp_usbCamera
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_usbCamera PROPERTY FOLDER "Plugins/Device")

endif()
