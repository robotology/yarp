# Copyright: (C) 2015
# Authors: Alberto Cardellino
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


yarp_prepare_plugin(usbCamera
                    CATEGORY device
                    TYPE yarp::dev::USBCameraDriverRgb
                    INCLUDE USBcamera.hpp
                    EXTRA_CONFIG WRAPPER=grabberDual)

# usbCameraRaw is always enabled when USBcamera is enabled
# This is a workaround to enable the creation of the additional source files and to hide the entry in ccmake
set(ENABLE_icubmod_usbCameraRaw ${ENABLE_icubmod_usbCamera} CACHE INTERNAL "Enable/disable compilation of icubmod_USBcameraRaw")
yarp_prepare_plugin(usbCameraRaw
                    CATEGORY device
                    TYPE yarp::dev::USBCameraDriverRaw
                    INCLUDE USBcamera.hpp
                    EXTRA_CONFIG WRAPPER=grabberDual)

IF(ENABLE_icubmod_usbCamera)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        MESSAGE(SEND_ERROR "USBcamera: device not available yet on OS X platform, please turn off the device")
        return()
    elseif(UNIX)
        find_package(Libv4l2 REQUIRED)
        find_package(Libv4lconvert REQUIRED)

        if( NOT Libv4l2_FOUND )
            MESSAGE(SEND_ERROR "USBcamera: library libv4l2 not found, please turn off device. You can install this dependency with 'sudo apt-get install libv4l-dev'")
            return()
        endif(NOT Libv4l2_FOUND )

        if( NOT Libv4lconvert_FOUND )
            MESSAGE(SEND_ERROR "USBcamera: libv4lconvert not found, please turn off device. You can install this dependency with 'sudo apt-get install libv4l-dev'")
            return()
        endif(NOT Libv4lconvert_FOUND )

        if(NOT ICUB_USE_OpenCV)
            MESSAGE(SEND_ERROR "USBcamera: opencv not found, please turn off device. You can install this dependency with 'sudo apt-get install libopencv-dev'")
            return()
        endif(NOT ICUB_USE_OpenCV )

    elseif(WIN32)
        MESSAGE(SEND_ERROR "USBcamera: device not available yet on Windows platform, please turn off the device")
        return()
    else()
        MESSAGE(SEND_ERROR "USBcamera: OS not supported, please turn off the device")
        return()
  endif()

    set(USBcamera_common_SRCS  common/USBcamera.cpp)
    set(USBcamera_common_HDRS  common/USBcamera.hpp)

    if(UNIX)
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/linux ${Libv4l2_INCLUDE_DIRS} ${Libv4lconvert_INCLUDE_DIRS})

        set(OS_SOURCES linux/V4L_camera.hpp linux/V4L_camera.cpp linux/list.hpp linux/list.cpp)
        # Files from leopard sdk (not used right now)--> linux/Leopard_MT9M021C.cpp linux/raw2bmp.cpp
    else()
#        include_directories(${SOME_INCLUDE_DIRS})
#        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/windows)
#        set(OS_SOURCES windows/camera.hpp windows/camera.cpp)
    endif()

    # includes valid for all platforms
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)

    yarp_add_plugin(usbCamera ${USBcamera_common_SRCS} ${USBcamera_common_HDRS} ${OS_SOURCES})
    target_link_libraries(usbCamera ${YARP_LIBRARIES})

    if(UNIX)
        target_link_libraries(usbCamera ${Libv4l2_LIBRARIES} ${Libv4lconvert_LIBRARIES} ${OpenCV_LIBRARIES})
    elseif(WIN32)
#       target_link_libraries(usbCamera ${SOME_WIN_LIBRARIES})
    else()
        MESSAGE(SEND_ERROR "USBcamera: OS not supported, please turn off the device")
    endif()

    icub_export_plugin(usbCamera)
    yarp_install(FILES usbCamera.ini  DESTINATION ${ICUB_PLUGIN_MANIFESTS_INSTALL_DIR})
    yarp_install(FILES usbCameraRaw.ini  DESTINATION ${ICUB_PLUGIN_MANIFESTS_INSTALL_DIR})

ENDIF(ENABLE_icubmod_usbCamera)









