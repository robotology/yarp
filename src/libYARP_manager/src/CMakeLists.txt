# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# FIXME All API should use a YARP_manager_API for __declspec(dllimport/dllexport)
# For now always build the library as STATIC
add_library(YARP_manager STATIC)
add_library(YARP::YARP_manager ALIAS YARP_manager)

set(YARP_manager_HDRS yarp/manager/application.h
                      yarp/manager/arbitrator.h
                      yarp/manager/binexparser.h
                      yarp/manager/broker.h
                      yarp/manager/data.h
                      yarp/manager/execstate.h
                      yarp/manager/executable.h
                      yarp/manager/fsm.h
                      yarp/manager/graph.h
                      yarp/manager/kbase.h
                      yarp/manager/localbroker.h
                      yarp/manager/logicresource.h
                      yarp/manager/manager.h
                      yarp/manager/manifestloader.h
                      yarp/manager/module.h
                      yarp/manager/node.h
                      yarp/manager/physicresource.h
                      yarp/manager/primresource.h
                      yarp/manager/resource.h
                      yarp/manager/scriptbroker.h
                      yarp/manager/singleapploader.h
                      yarp/manager/utility.h
                      yarp/manager/xmlapploader.h
                      yarp/manager/xmlclusterloader.h
                      yarp/manager/xmlappsaver.h
                      yarp/manager/xmlmodloader.h
                      yarp/manager/xmlresloader.h
                      yarp/manager/xmltemploader.h
                      yarp/manager/yarpbroker.h
                      yarp/manager/yarpdevbroker.h
                      yarp/manager/ymm-types.h)

set(YARP_manager_IMPL_HDRS yarp/manager/impl/textparser.h)

set(YARP_manager_SRCS yarp/manager/application.cpp
                      yarp/manager/arbitrator.cpp
                      yarp/manager/binexparser.cpp
                      yarp/manager/broker.cpp
                      yarp/manager/data.cpp
                      yarp/manager/execstate.cpp
                      yarp/manager/executable.cpp
                      yarp/manager/graph.cpp
                      yarp/manager/kbase.cpp
                      yarp/manager/localbroker.cpp
                      yarp/manager/logicresource.cpp
                      yarp/manager/manager.cpp
                      yarp/manager/module.cpp
                      yarp/manager/node.cpp
                      yarp/manager/physicresource.cpp
                      yarp/manager/primresource.cpp
                      yarp/manager/resource.cpp
                      yarp/manager/scriptbroker.cpp
                      yarp/manager/singleapploader.cpp
                      yarp/manager/utility.cpp
                      yarp/manager/xmlapploader.cpp
                      yarp/manager/xmlclusterloader.cpp
                      yarp/manager/xmlappsaver.cpp
                      yarp/manager/xmlmodloader.cpp
                      yarp/manager/xmlresloader.cpp
                      yarp/manager/xmltemploader.cpp
                      yarp/manager/yarpbroker.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_manager_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_manager_HDRS}
                   ${YARP_manager_IMPL_HDRS})

target_sources(YARP_manager PRIVATE ${YARP_manager_SRCS}
                                    ${YARP_manager_HDRS}
                                    ${YARP_manager_IMPL_HDRS})

target_include_directories(YARP_manager PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                               $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
if(MSVC)
  target_include_directories(YARP_manager SYSTEM PRIVATE ${dirent_INCLUDE_DIRS})
endif()
target_compile_features(YARP_manager PUBLIC cxx_std_14)

target_link_libraries(YARP_manager PUBLIC YARP::YARP_os
                                   PRIVATE YARP::YARP_sig)
list(APPEND YARP_manager_PUBLIC_DEPS YARP_os)
list(APPEND YARP_manager_PRIVATE_DEPS YARP_sig)

if(TARGET YARP::YARP_math)
  target_link_libraries(YARP_manager PRIVATE YARP::YARP_math)
  target_compile_definitions(YARP_manager PRIVATE WITH_YARPMATH)
  list(APPEND YARP_manager_PRIVATE_DEPS YARP_math)
endif()

target_include_directories(YARP_manager SYSTEM PRIVATE ${TinyXML_INCLUDE_DIRS})
target_link_libraries(YARP_manager PRIVATE ${TinyXML_LIBRARIES})
list(APPEND YARP_manager_PRIVATE_DEPS TinyXML)

set_property(TARGET YARP_manager PROPERTY PUBLIC_HEADER ${YARP_manager_HDRS})
set_property(TARGET YARP_manager PROPERTY PRIVATE_HEADER ${YARP_manager_IMPL_HDRS})
set_property(TARGET YARP_manager PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_manager PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_manager PROPERTY FOLDER "Libraries/Private")


install(TARGETS YARP_manager
        EXPORT YARP_manager
        RUNTIME
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT YARP_manager
        LIBRARY
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_manager
          NAMELINK_COMPONENT YARP_manager-dev
        ARCHIVE
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_manager-dev
        PUBLIC_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/manager"
          COMPONENT YARP_manager-dev
        PRIVATE_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/manager/impl"
          COMPONENT YARP_manager-priv-dev)


set(YARP_manager_PUBLIC_DEPS ${YARP_manager_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_manager_PRIVATE_DEPS ${YARP_manager_PRIVATE_DEPS} PARENT_SCOPE)
