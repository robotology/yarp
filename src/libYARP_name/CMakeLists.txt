# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (YARP_USE_PERSISTENT_NAMESERVER)

    project(YARP_name)

    set(YARP_name_HDRS include/yarp/name/all.h
                       include/yarp/name/api.h
                       include/yarp/name/BootstrapServer.h
                       include/yarp/name/NameServerConnectionHandler.h
                       include/yarp/name/NameServerManager.h
                       include/yarp/name/NameService.h)

    set(YARP_name_SRCS src/BootstrapServer.cpp
                       src/NameService.cpp)

    source_group("Source Files" FILES ${YARP_name_SRCS})
    source_group("Header Files" FILES ${YARP_name_HDRS})

    set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS
                    ${PROJECT_SOURCE_DIR}/include)
    get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
    include_directories(${YARP_TREE_INCLUDE_DIRS})
    include_directories(${ACE_INCLUDE_DIRS})

    add_library(YARP_name ${YARP_name_SRCS} ${YARP_name_HDRS})
    target_link_libraries(YARP_name LINK_PRIVATE YARP_OS)
    target_link_libraries(YARP_name LINK_PRIVATE ${ACE_LIBRARIES})
    install(TARGETS YARP_name EXPORT YARP COMPONENT runtime DESTINATION lib${LIB_SUFFIX})
    set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_name)

    set_property(TARGET YARP_name PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})

    install(FILES ${YARP_name_HDRS}
            COMPONENT development
            DESTINATION include/yarp/name)
endif()

# touch 1
