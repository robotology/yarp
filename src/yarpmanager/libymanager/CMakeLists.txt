#  Yarp Modules Manager
#  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

project(YARP_manager)

set(folder_source   src/application.cpp
                    src/arbitrator.cpp
                    src/binexparser.cpp
                    src/broker.cpp
                    src/data.cpp
                    src/execstate.cpp
                    src/executable.cpp
                    src/graph.cpp
                    src/kbase.cpp
                    src/localbroker.cpp
                    src/logicresource.cpp
                    src/manager.cpp
                    src/module.cpp
                    src/node.cpp
                    src/physicresource.cpp
                    src/primresource.cpp
                    src/resource.cpp
                    src/scriptbroker.cpp
                    src/singleapploader.cpp
                    src/utility.cpp
                    src/xmlapploader.cpp
                    src/xmlappsaver.cpp
                    src/xmlmodloader.cpp
                    src/xmlresloader.cpp
                    src/xmltemploader.cpp
                    src/yarpbroker.cpp)

set(folder_header   include/yarp/manager/application.h
                    include/yarp/manager/arbitrator.h
                    include/yarp/manager/binexparser.h
                    include/yarp/manager/broker.h
                    include/yarp/manager/data.h
                    include/yarp/manager/execstate.h
                    include/yarp/manager/executable.h
                    include/yarp/manager/fsm.h
                    include/yarp/manager/graph.h
                    include/yarp/manager/kbase.h
                    include/yarp/manager/localbroker.h
                    include/yarp/manager/logicresource.h
                    include/yarp/manager/manager.h
                    include/yarp/manager/manifestloader.h
                    include/yarp/manager/module.h
                    include/yarp/manager/node.h
                    include/yarp/manager/physicresource.h
                    include/yarp/manager/primresource.h
                    include/yarp/manager/resource.h
                    include/yarp/manager/scriptbroker.h
                    include/yarp/manager/singleapploader.h
                    include/yarp/manager/utility.h
                    include/yarp/manager/xmlapploader.h
                    include/yarp/manager/xmlappsaver.h
                    include/yarp/manager/xmlmodloader.h
                    include/yarp/manager/xmlresloader.h
                    include/yarp/manager/xmltemploader.h
                    include/yarp/manager/yarpbroker.h
                    include/yarp/manager/yarpdevbroker.h
                    include/yarp/manager/ymm-dir.h
                    include/yarp/manager/ymm-types.h)

source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
  ${PROJECT_SOURCE_DIR}/include)
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(${TinyXML_INCLUDE_DIRS})


if(CREATE_LIB_MATH)
    add_definitions( -DWITH_YARPMATH )
endif()


option(YARP_EXPERIMENTAL_MANAGER_LIB "Enable experimental installation of YARP Manager Library" FALSE)
mark_as_advanced(YARP_EXPERIMENTAL_MANAGER_LIB)

if(${CMAKE_VERSION} VERSION_LESS 2.8.8  AND  YARP_EXPERIMENTAL_MANAGER_LIB)
    message("YARP Manager Library requires CMake 2.8.8 or later. Disabling YARP_EXPERIMENTAL_MANAGER_LIB")
    set_property(CACHE YARP_EXPERIMENTAL_MANAGER_LIB PROPERTY VALUE FALSE)
endif()


if(NOT YARP_EXPERIMENTAL_MANAGER_LIB)
    add_library(YARP_manager STATIC ${folder_source} ${folder_header})
else()
    add_library(YARP_manager ${folder_source} ${folder_header})
endif()

target_link_libraries(YARP_manager LINK_PRIVATE YARP_sig YARP_OS)
target_link_libraries(YARP_manager LINK_PRIVATE ${TinyXML_LIBRARIES})

if(CREATE_LIB_MATH)
    target_link_libraries(YARP_manager LINK_PRIVATE YARP_math)
endif()



if(YARP_EXPERIMENTAL_MANAGER_LIB)
    set_target_properties(YARP_manager PROPERTIES VERSION ${YARP_VERSION}
                                                  SOVERSION ${YARP_GENERIC_SOVERSION})

    # Path for installed cmake files
    if(WIN32 AND NOT CYGWIN)
        set(YARP_MANAGER_CMAKE_DESTINATION CMake)
    else()
        set(YARP_MANAGER_CMAKE_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/YARPManager)
    endif()

    install(TARGETS YARP_manager
            EXPORT YARPManager
            COMPONENT runtime
            DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(DIRECTORY include/yarp/manager
            COMPONENT development
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp
            FILES_MATCHING PATTERN "*.h"
            PATTERN ".*" EXCLUDE)

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(${CMAKE_BINARY_DIR}/YARPManagerConfigVersion.cmake
                                     VERSION ${YARP_VERSION}
                                     COMPATIBILITY AnyNewerVersion)

    install(FILES ${CMAKE_BINARY_DIR}/YARPManagerConfigVersion.cmake
            DESTINATION ${YARP_MANAGER_CMAKE_DESTINATION})


    # YARPManagerConfig.cmake (build tree)
    get_filename_component(YARP_MANAGER_INCLUDEDIR ${CMAKE_CURRENT_SOURCE_DIR}/include/ ABSOLUTE)
    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/YARPManagerConfig.cmake.in
                                  ${CMAKE_BINARY_DIR}/YARPManagerConfig.cmake
                                  INSTALL_DESTINATION ${CMAKE_BINARY_DIR}
                                  PATH_VARS YARP_MANAGER_INCLUDEDIR
                                  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

    # YARPManagerConfig.cmake (installed)
    set(YARP_MANAGER_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR})
    configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/YARPManagerConfig.cmake.in
                                  ${CMAKE_BINARY_DIR}/YARPManagerConfig.cmake.install
                                  INSTALL_DESTINATION ${YARP_MANAGER_CMAKE_DESTINATION}
                                  PATH_VARS YARP_MANAGER_INCLUDEDIR
                                  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

    install(FILES ${CMAKE_BINARY_DIR}/YARPManagerConfig.cmake.install
            DESTINATION ${YARP_MANAGER_CMAKE_DESTINATION}
            RENAME YARPManagerConfig.cmake)

    # This is a workaround for exporting when using static libraries
    if(NOT CREATE_SHARED_LIBRARY)
        set(manager_extra_libs YARP_sig YARP_OS)
        if(CREATE_LIB_MATH)
            list(APPEND manager_extra_libs YARP_math)
        endif()
    endif()
    # TinyXML is always built static
    if(NOT YARP_USE_SYSTEM_TINYXML)
        list(APPEND manager_extra_libs ${TinyXML_LIBRARIES})
    endif()
    set_property(TARGET YARP_manager PROPERTY LINK_LIBRARIES "${manager_extra_libs}")

    # YARPManagerTargets.cmake (build tree)
    export(TARGETS YARP_manager ${manager_extra_libs}
           NAMESPACE YARP::
           FILE ${CMAKE_BINARY_DIR}/YARPManagerTargets.cmake)

    # YARPManagerTargets.cmake (installed)
    install(EXPORT YARPManager
            NAMESPACE YARP::
            DESTINATION ${YARP_MANAGER_CMAKE_DESTINATION}
            FILE YARPManagerTargets.cmake)
endif()
