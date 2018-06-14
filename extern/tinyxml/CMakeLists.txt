# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# TinyXML
project(YARP_priv_tinyxml)

set(tinyxml_SRCS
    tinyxml/tinyxml.cpp
    tinyxml/tinyxmlerror.cpp
    tinyxml/tinyxmlparser.cpp
)

set(tinyxml_HDRS
    tinyxml/tinystr.h
    tinyxml/tinyxml.h
)

add_definitions("-DTIXML_USE_STL")

add_library(YARP_priv_tinyxml STATIC ${tinyxml_SRCS})

set_property(TARGET YARP_priv_tinyxml PROPERTY FOLDER "Libraries/External")

set(TinyXML_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/tinyxml PARENT_SCOPE)
set(TinyXML_LIBRARIES "YARP_priv_tinyxml" PARENT_SCOPE)
set(TinyXML_DEFINITIONS "-DTIXML_USE_STL" PARENT_SCOPE)

install(TARGETS YARP_priv_tinyxml
        EXPORT YARP_priv_tinyxml
        COMPONENT YARP_priv_tinyxml
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

if(NOT CREATE_SHARED_LIBS)
  include(YarpInstallBasicPackageFiles)
  yarp_install_basic_package_files(YARP_priv_tinyxml)
endif()
