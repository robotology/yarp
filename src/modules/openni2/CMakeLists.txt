# Copyright (C) 2013
# Author: Konstantinos Theofilis
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_DEVICE_LIBRARY)
    yarp_prepare_device(OpenNI2DeviceServer TYPE OpenNI2DeviceDriverServer INCLUDE OpenNI2DeviceDriverServer.h)
    yarp_prepare_device(OpenNI2DeviceClient TYPE OpenNI2DeviceDriverClient INCLUDE OpenNI2DeviceDriverClient.h)
    yarp_add_device_fingerprint(openni2.ini OpenNI2DeviceServer OpenNI2DeviceClient)
endif()

if (SKIP_OpenNI2DeviceServer AND SKIP_OpenNI2DeviceClient)
    set(SKIP_OpenNI2Device TRUE)
endif()

if (NOT SKIP_OpenNI2Device)

    find_package(YARP REQUIRED)

    find_path(OPENNI2_INCLUDE_LOCAL NAMES OpenNI.h HINTS $ENV{OPENNI2_INCLUDE})
    find_library(OPENNI2_LIBRARY NAMES OpenNI2 libOpenNI2.dylib libOpenNI2 HINTS $ENV{OPENNI2_REDIST})
    find_path(NITE2_INCLUDE_LOCAL NAMES NiTE.h HINTS $ENV{NITE2_INCLUDE})
    find_library(NITE2_LIBRARY NAMES NiTE2 HINTS $ENV{NITE2_REDIST64})


    include_directories(${OPENNI2_INCLUDE_LOCAL} ${NITE2_INCLUDE_LOCAL} ${YARP_INCLUDE_DIRS})
    link_libraries(${OPENNI2_LIBRARY} ${NITE2_LIBRARY} ${YARP_LIBRARIES})

    # Search for source code.
    file(GLOB folder_source *.cpp *.cc *.c)
    file(GLOB folder_header *.h)
    source_group("Source Files" FILES ${folder_source})
    source_group("Header Files" FILES ${folder_header})

    yarp_add_plugin(yarp_OpenNI2Device ${folder_source} ${folder_header})
    target_link_libraries(yarp_OpenNI2Device ${YARP_LIBRARIES} ${LINK_LIBRARIES})
endif()




