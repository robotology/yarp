# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# QGVCore
project(YARP_priv_qgvcore)

include(YarpUseQt5)

set(qgvcore_SRCS qgv/QGVCore/private/QGVCore.cpp
                 qgv/QGVCore/private/QGVGraphPrivate.cpp
                 qgv/QGVCore/private/QGVEdgePrivate.cpp
                 qgv/QGVCore/private/QGVGvcPrivate.cpp
                 qgv/QGVCore/private/QGVNodePrivate.cpp
                 qgv/QGVCore/QGVEdge.cpp
                 qgv/QGVCore/QGVNode.cpp
                 qgv/QGVCore/QGVScene.cpp
                 qgv/QGVCore/QGVSubGraph.cpp)


set(qgvcore_HDRS qgv/QGVCore/qgv.h
                 qgv/QGVCore/private/QGVCore.h
                 qgv/QGVCore/private/QGVGraphPrivate.h
                 qgv/QGVCore/private/QGVNodePrivate.h
                 qgv/QGVCore/private/QGVEdgePrivate.h
                 qgv/QGVCore/private/QGVGvcPrivate.h
                 qgv/QGVCore/QGVEdge.h
                 qgv/QGVCore/QGVNode.h
                 qgv/QGVCore/QGVScene.h
                 qgv/QGVCore/QGVSubGraph.h)

add_definitions(-DQGVCORE_LIB -DWITH_CGRAPH)

if(NOT MSVC)
  check_cxx_compiler_flag("-Wno-undef" CXX_HAS_WNO_UNDEF)
  if(CXX_HAS_WNO_UNDEF)
    set_property(SOURCE ${qgvcore_SRCS} APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-undef")
  endif()
  check_cxx_compiler_flag("-Wno-reorder" CXX_HAS_WNO_REORDER)
  if(CXX_HAS_WNO_REORDER)
    set_property(SOURCE ${qgvcore_SRCS} APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-reorder")
  endif()
  check_cxx_compiler_flag("-Wno-unused-parameter" CXX_HAS_WNO_UNUSED_PARAMETER)
  if(CXX_HAS_WNO_UNUSED_PARAMETER)
    set_property(SOURCE ${qgvcore_SRCS} APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-unused-parameter")
  endif()
  check_cxx_compiler_flag("-Wno-redundant-decls" CXX_HAS_WNO_REDUNDANT_DECLS)
  if(CXX_HAS_WNO_REDUNDANT_DECLS)
    set_property(SOURCE ${qgvcore_SRCS} APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-redundant-decls")
  endif()
endif()

include_directories(${Graphviz_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/qgv/QGVCore
                    ${CMAKE_CURRENT_SOURCE_DIR}/qgv/QGVCore/private
                    ${CMAKE_CURRENT_BINARY_DIR})

add_library(YARP_priv_qgvcore STATIC ${qgvcore_SRCS} ${qgvcore_HDRS})

target_link_libraries(YARP_priv_qgvcore Qt5::Widgets
                                        ${Graphviz_LIBRARIES})

set_property(TARGET YARP_priv_qgvcore PROPERTY FOLDER "Libraries/External")

set(QGVCore_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/qgv/QGVCore PARENT_SCOPE)
set(QGVCore_LIBRARIES "YARP_priv_qgvcore" PARENT_SCOPE)
set(QGVCore_DEFINITIONS "" PARENT_SCOPE)

install(TARGETS YARP_priv_qgvcore
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_priv_qgvcore)
