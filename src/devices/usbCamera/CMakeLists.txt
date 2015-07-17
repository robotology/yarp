# Copyright: (C) 2015
# Authors: Alberto Cardellino
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


yarp_prepare_device(USBcamera TYPE yarp::dev::USBCameraDriverRgb INCLUDE USBcamera.hpp WRAPPER grabber)

#include(Findlibjpeg.cmake)
#find_library(ljpeg)


# usbCameraRaw is always enabled when USBcamera is enabled
# This is a workaround to enable the creation of the additional source files and to hide the entry in ccmake
set(ENABLE_icubmod_USBcameraRaw ${ENABLE_icubmod_USBcamera} CACHE INTERNAL "Enable/disable compilation of icubmod_USBcameraRaw")
yarp_prepare_device(USBcameraRaw TYPE yarp::dev::USBCameraDriverRaw INCLUDE USBcamera.hpp WRAPPER grabber)

IF(ENABLE_icubmod_USBcamera)

  if(WIN32)  # e win 64??
    MESSAGE(SEND_ERROR "USBcamera: device not available yet on Windows platform, please turn off the device")
    return()

  else()  # Linux  & MAC

    include(Findlibv4l2.cmake)
    find_package(LibV4L2 REQUIRED)

    if( NOT LIBV4L2_FOUND )
        MESSAGE(SEND_ERROR "USBcamera: LibV4L2 api not found, turn off device")
        return()
    endif(NOT LIBV4L2_FOUND )

  endif(WIN32)

    set(USBcamera_common_SRCS  common/USBcamera.cpp)
    set(USBcamera_common_HDRS  common/USBcamera.hpp)

    if(WIN32)
#         INCLUDE_DIRECTORIES(${SOME_INCLUDE_DIRS})
        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/windows)
        set(OS_SOURCES windows/camera.hpp windows/camera.cpp)
    else()
#         INCLUDE_DIRECTORIES(${SOME_INCLUDE_DIRS})
        INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/linux)
#         set(OS_SOURCES linux/V4L_camera.hpp linux/V4L_camera.cpp)# linux/list.hpp linux/list.cpp)
    endif(WIN32)

    add_definitions("-Wall -Wextra")
    #add_definitions("--ansi --pedantic")
    #add_definitions("-Wno-long-long -Wno-variadic-macros -Wshadow")

    include_directories(${YARP_INCLUDE_DIRS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)


    yarp_add_plugin(USBcamera ${USBcamera_common_SRCS} ${USBcamera_common_HDRS} ${OS_SOURCES})
    target_link_libraries(USBcamera ${YARP_LIBRARIES})

    if(WIN32)
#     target_link_libraries(USBcamera ${SOME_WIN_LIBRARIES})
    else()
        target_link_libraries(USBcamera v4l2)
    endif(WIN32)

    icub_export_plugin(USBcamera)
    yarp_install(FILES USBcamera.ini  DESTINATION ${ICUB_PLUGIN_MANIFESTS_INSTALL_DIR})
    yarp_install(FILES USBcameraRaw.ini  DESTINATION ${ICUB_PLUGIN_MANIFESTS_INSTALL_DIR})

ENDIF(ENABLE_icubmod_USBcamera)









