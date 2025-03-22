# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# TinyXML
project(YARP_priv_TinyXML)

add_library(YARP_priv_TinyXML STATIC)

set(tinyxml_SRCS
  tinyxml/tinyxml.cpp
  tinyxml/tinyxmlerror.cpp
  tinyxml/tinyxmlparser.cpp
)

set(tinyxml_HDRS
  tinyxml/tinystr.h
  tinyxml/tinyxml.h
)

target_sources(YARP_priv_TinyXML PRIVATE ${tinyxml_SRCS})

target_include_directories(YARP_priv_TinyXML PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tinyxml>)

target_compile_definitions(YARP_priv_TinyXML PUBLIC "TIXML_USE_STL")

set_property(TARGET YARP_priv_TinyXML PROPERTY FOLDER "Libraries/External")

set(TinyXML_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/tinyxml PARENT_SCOPE)
set(TinyXML_LIBRARIES "YARP_priv_TinyXML" PARENT_SCOPE)
set(TinyXML_DEFINITIONS "-DTIXML_USE_STL" PARENT_SCOPE)

install(
  TARGETS YARP_priv_TinyXML
  EXPORT YARP_priv_TinyXML
  COMPONENT YARP_priv_TinyXML
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

if(NOT CREATE_SHARED_LIBS)
  include(YarpInstallBasicPackageFiles)
  yarp_install_basic_package_files(YARP_priv_TinyXML)
endif()
