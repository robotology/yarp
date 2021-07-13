# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

project(ContainerExample)


find_package(Qt5 COMPONENTS Core Widgets Quick Qml REQUIRED)


# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)


# Remove -rdynamic from CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS.
# This is needed for some weird reason, or executables will not load resources
# from the plugins.
string(REPLACE "-rdynamic" "" CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS}")


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qtquick2applicationviewer)

file(GLOB_RECURSE ContainerExample_SRCS *.cpp)
file(GLOB_RECURSE ContainerExample_HDRS *.h)
file(GLOB ContainerExample_QRC_FILES *.qrc)

qt5_add_resources(ContainerExample_QRC_GEN_FILES ${ContainerExample_QRC_FILES})

source_group(
  "Resources Files"
  FILES ${ContainerExample_QRC_FILES}
)
source_group(
  "Generated Files"
  FILES ${ContainerExample_QRC_GEN_FILES}
)

add_executable(ContainerExample WIN32)
target_sources(ContainerExample
  PRIVATE
    ${ContainerExample_SRCS}
    ${ContainerExample_HDRS}
    ${ContainerExample_QRC_GEN_FILES}
)
target_link_libraries(ContainerExample
  PRIVATE
    Qt5::Widgets
    Qt5::Qml
    Qt5::Quick
)


#-------------------------------------------------------------------------------
# YARP is not actually required to build the project, but we use it here to
# print an useful reminder to set the QML2_IMPORT_PATH environment variable

find_package(YARP)

set(_qml2_import_path "${YARP_INSTALL_PREFIX}/${YARP_QML2_IMPORT_DIR}")
file(TO_NATIVE_PATH "${_qml2_import_path}" PLUGINS_RELATIVE_PATH)
message("YARP Qt5 plugins are installed in \n
    ${_qml2_import_path}\n
If they are not found automatically, you probably need to add this directory to your\n
    QML2_IMPORT_PATH\n
environment variable")
