# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(YARP_COMPILE_libYARP_manager)

  project(YARP_manager)

  set(YARP_manager_HDRS include/yarp/manager/application.h
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
                        include/yarp/manager/xmlclusterloader.h
                        include/yarp/manager/xmlappsaver.h
                        include/yarp/manager/xmlmodloader.h
                        include/yarp/manager/xmlresloader.h
                        include/yarp/manager/xmltemploader.h
                        include/yarp/manager/yarpbroker.h
                        include/yarp/manager/yarpdevbroker.h
                        include/yarp/manager/ymm-types.h)

  set(YARP_manager_IMPL_HDRS include/yarp/manager/impl/textparser.h)

  set(YARP_manager_SRCS src/application.cpp
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
                        src/xmlclusterloader.cpp
                        src/xmlappsaver.cpp
                        src/xmlmodloader.cpp
                        src/xmlresloader.cpp
                        src/xmltemploader.cpp
                        src/yarpbroker.cpp)

  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
               PREFIX "Source Files"
               FILES ${YARP_manager_SRCS})
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
               PREFIX "Header Files"
               FILES ${YARP_manager_HDRS}
                     ${YARP_manager_IMPL_HDRS})


  # FIXME All API should use a YARP_manager_API for __declspec(dllimport/dllexport)
  # For now always build the library as STATIC
  add_library(YARP_manager STATIC ${YARP_manager_SRCS}
                                  ${YARP_manager_HDRS}
                                  ${YARP_manager_IMPL_HDRS})
  add_library(YARP::YARP_manager ALIAS YARP_manager)

  target_include_directories(YARP_manager PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                 $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  if(MSVC)
    target_include_directories(YARP_manager SYSTEM PRIVATE ${dirent_INCLUDE_DIRS})
  endif()

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

  target_compile_features(YARP_manager PUBLIC cxx_override)

  set_property(TARGET YARP_manager PROPERTY PUBLIC_HEADER ${YARP_manager_HDRS})
  set_property(TARGET YARP_manager PROPERTY PRIVATE_HEADER ${YARP_manager_IMPL_HDRS})
  set_property(TARGET YARP_manager PROPERTY VERSION ${YARP_VERSION_SHORT})
  set_property(TARGET YARP_manager PROPERTY SOVERSION ${YARP_SOVERSION})

  install(TARGETS YARP_manager
          EXPORT YARP_manager
          COMPONENT YARP_manager
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/manager"
          PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/manager/impl")

  include(YarpInstallBasicPackageFiles)
  yarp_install_basic_package_files(YARP_manager
                                   DEPENDENCIES ${YARP_manager_PUBLIC_DEPS}
                                   PRIVATE_DEPENDENCIES ${YARP_manager_PRIVATE_DEPS})

  set_property(TARGET YARP_manager PROPERTY FOLDER "Libraries/Private")
endif()
