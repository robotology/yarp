# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# TinyXML
project(tinyxml)

set(tinyxml_SRCS
    tinyxml/tinystr.cpp
    tinyxml/tinyxml.cpp
    tinyxml/tinyxmlerror.cpp
    tinyxml/tinyxmlparser.cpp
)

set(tinyxml_HDRS
    tinyxml/tinystr.h
    tinyxml/tinyxml.h
)

add_library(tinyxml STATIC ${tinyxml_SRCS})


set(TINYXML_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/tinyxml" CACHE PATH "TinyXML include directory" FORCE)
set(TINYXML_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/lib" CACHE PATH "TinyXML library directory" FORCE)
set(TINYXML_LIBRARIES "tinyxml" CACHE STRING "TinyXML libraries" FORCE)
mark_as_advanced(TINYXML_INCLUDE_DIRS TINYXML_LIBRARY_DIRS TINYXML_LIBRARIES)
