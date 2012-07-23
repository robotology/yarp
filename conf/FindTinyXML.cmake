# Try to find the TinyXML library
# TinyXML_FOUND - system has TinyXML
# TinyXML_INCLUDE_DIRS - TinyXML include directory
# TinyXML_LIBRARY_DIRS - TinyXML library directory
# TinyXML_LIBRARIES - TinyXML libraries

# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


set(TinyXML_FIND_REQUIRED ${TinyXML_FIND_REQUIRED})
if(TinyXML_INCLUDE_DIR AND TinyXML_LIBRARIES)
    set(TINYXML_FIND_QUIETLY TRUE)
endif(TinyXML_INCLUDE_DIR AND TinyXML_LIBRARIES)


if(NOT WIN32)
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        if(TinyXML_FIND_VERSION)
            if(TinyXML_FIND_VERSION_EXACT)
                pkg_check_modules(PC_TINYXML QUIET tinyxml=${TinyXML_FIND_VERSION})
            else(TinyXML_FIND_VERSION_EXACT)
                pkg_check_modules(PC_TINYXML QUIET tinyxml>=${TinyXML_FIND_VERSION})
            endif(TinyXML_FIND_VERSION_EXACT)
        else(TinyXML_FIND_VERSION)
            pkg_check_modules(PC_TINYXML QUIET tinyxml)
        endif(TinyXML_FIND_VERSION)
    endif(PKG_CONFIG_FOUND)
endif(NOT WIN32)

set(TinyXML_INCLUDE_DIRS ${PC_TINYXML_INCLUDE_DIRS} CACHE PATH "TinyXML include directory" FORCE)
set(TinyXML_LIBRARY_DIRS ${PC_TINYXML_LIBRARY_DIRS} CACHE PATH "TinyXML library directory" FORCE)
set(TinyXML_LIBRARIES ${PC_TINYXML_LIBRARIES} CACHE STRING "TinyXML libraries" FORCE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TinyXML
                                  DEFAULT_MSG
                                  TinyXML_LIBRARIES
)

mark_as_advanced(TinyXML_INCLUDE_DIRS TinyXML_LIBRARY_DIRS TinyXML_LIBRARIES)
