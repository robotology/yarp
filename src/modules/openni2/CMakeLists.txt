# Copyright (C) 2013
# Author: Konstantinos Theofilis
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_DEVICE_LIBRARY)
    yarp_prepare_device(OpenNI2DeviceServer TYPE yarp::dev::OpenNI2DeviceDriverServer INCLUDE OpenNI2DeviceDriverServer.h)
    yarp_prepare_device(OpenNI2DeviceClient TYPE yarp::dev::OpenNI2DeviceDriverClient INCLUDE OpenNI2DeviceDriverClient.h)
    yarp_install(FILES openni2.ini
                 COMPONENT runtime
                 DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

if (SKIP_OpenNI2DeviceServer AND SKIP_OpenNI2DeviceClient)
    set(SKIP_OpenNI2Device TRUE)
endif()

if (NOT SKIP_OpenNI2Device)

    find_package(YARP REQUIRED)

    find_path(OPENNI2_INCLUDE_LOCAL NAMES OpenNI.h HINTS $ENV{OPENNI2_INCLUDE} /usr/include/openni2)
    find_library(OPENNI2_LIBRARY NAMES OpenNI2 libOpenNI2.dylib libOpenNI2 HINTS $ENV{OPENNI2_REDIST})
    find_path(NITE2_INCLUDE_LOCAL NAMES NiTE.h HINTS $ENV{NITE2_INCLUDE})
    find_library(NITE2_LIBRARY NAMES NiTE2 HINTS $ENV{NITE2_REDIST64})
    if(NITE2_INCLUDE_LOCAL)
        set(NITE2_FOUND TRUE)
    else()
        set(NITE2_FOUND FALSE)
    endif()

    if(NOT NITE2_FOUND)
        message(WARNING "OpenNI2Device: NITE2 library was not found, skeleton extraction feature will be skipped by driver. "
         "If you need this feature please install NITE2 library.")
    else()
        add_definitions(-DOPENNI2_DRIVER_USES_NITE2)
    endif()

    include_directories(${OPENNI2_INCLUDE_LOCAL} ${YARP_INCLUDE_DIRS})
    if(NITE2_FOUND)
        include_directories( ${NITE2_INCLUDE_LOCAL})
    endif()

    # Search for source code.
    file(GLOB folder_source *.cpp *.cc *.c)
    file(GLOB folder_header *.h)

    source_group("Source Files" FILES ${folder_source})
    source_group("Header Files" FILES ${folder_header})

    yarp_add_plugin(yarp_OpenNI2Device ${folder_source} ${folder_header})
    target_link_libraries(yarp_OpenNI2Device ${OPENNI2_LIBRARY} ${YARP_LIBRARIES})

    if(NITE2_FOUND)
        target_link_libraries(yarp_OpenNI2Device ${NITE2_LIBRARY})
    endif()

    yarp_install(TARGETS yarp_OpenNI2Device
                 EXPORT YARP
                 COMPONENT runtime
                 LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

endif()




