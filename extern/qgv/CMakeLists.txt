# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# QGVCore
project(YARP_priv_qgvcore)

include(YarpUseQt5)

add_library(YARP_priv_qgvcore STATIC)

set(qgvcore_SRCS
  qgv/QGVCore/private/QGVCore.cpp
  qgv/QGVCore/private/QGVGraphPrivate.cpp
  qgv/QGVCore/private/QGVEdgePrivate.cpp
  qgv/QGVCore/private/QGVGvcPrivate.cpp
  qgv/QGVCore/private/QGVNodePrivate.cpp
  qgv/QGVCore/QGVEdge.cpp
  qgv/QGVCore/QGVNode.cpp
  qgv/QGVCore/QGVScene.cpp
  qgv/QGVCore/QGVSubGraph.cpp
)

set(qgvcore_HDRS
  qgv/QGVCore/qgv.h
  qgv/QGVCore/private/QGVCore.h
  qgv/QGVCore/private/QGVGraphPrivate.h
  qgv/QGVCore/private/QGVNodePrivate.h
  qgv/QGVCore/private/QGVEdgePrivate.h
  qgv/QGVCore/private/QGVGvcPrivate.h
  qgv/QGVCore/QGVEdge.h
  qgv/QGVCore/QGVNode.h
  qgv/QGVCore/QGVScene.h
  qgv/QGVCore/QGVSubGraph.h
)

target_sources(YARP_priv_qgvcore PRIVATE ${qgvcore_SRCS} ${qgvcore_HDRS})

target_compile_definitions(YARP_priv_qgvcore PRIVATE QGVCORE_LIB)

target_include_directories(YARP_priv_qgvcore
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qgv/QGVCore>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/qgv/QGVCore/private
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(YARP_priv_qgvcore PUBLIC Qt5::Widgets)

target_include_directories(YARP_priv_qgvcore PRIVATE ${Graphviz_INCLUDE_DIR})
target_link_libraries(YARP_priv_qgvcore PRIVATE ${Graphviz_LIBRARIES})

set_property(TARGET YARP_priv_qgvcore PROPERTY FOLDER "Libraries/External")


set(QGVCore_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/qgv/QGVCore PARENT_SCOPE)
set(QGVCore_LIBRARIES "YARP_priv_qgvcore" PARENT_SCOPE)
set(QGVCore_DEFINITIONS "" PARENT_SCOPE)

install(
  TARGETS YARP_priv_qgvcore
  EXPORT YARP_priv_qgvcore
  COMPONENT YARP_priv_qgvcore
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

if(NOT CREATE_SHARED_LIBS)
  include(YarpInstallBasicPackageFiles)
  yarp_install_basic_package_files(YARP_priv_qgvcore)
endif()
