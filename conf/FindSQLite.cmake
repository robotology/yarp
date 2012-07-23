# Try to find the SQLite 3 library
# SQLite_FOUND - system has SQLite
# SQLite_INCLUDE_DIRS - SQLite include directory
# SQLite_LIBRARY_DIRS - SQLite library directory
# SQLite_LIBRARIES - SQLite libraries

# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


set(SQLite_FIND_REQUIRED ${SQLite_FIND_REQUIRED})
if(SQLite_INCLUDE_DIR AND SQLite_LIBRARIES)
    set(SQLITE_FIND_QUIETLY TRUE)
endif(SQLite_INCLUDE_DIR AND SQLite_LIBRARIES)


if(NOT WIN32)
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        if(SQLite_FIND_VERSION)
            if(SQLite_FIND_VERSION_EXACT)
                pkg_check_modules(PC_SQLITE QUIET sqlite3=${SQLite_FIND_VERSION})
            else(SQLite_FIND_VERSION_EXACT)
                pkg_check_modules(PC_SQLITE QUIET sqlite3>=${SQLite_FIND_VERSION})
            endif(SQLite_FIND_VERSION_EXACT)
        else(SQLite_FIND_VERSION)
            pkg_check_modules(PC_SQLITE QUIET sqlite3)
        endif(SQLite_FIND_VERSION)
    endif(PKG_CONFIG_FOUND)
endif(NOT WIN32)

set(SQLite_INCLUDE_DIRS ${PC_SQLITE_INCLUDE_DIRS} CACHE PATH "SQLite include directory" FORCE)
set(SQLite_LIBRARY_DIRS ${PC_SQLITE_LIBRARY_DIRS} CACHE PATH "SQLite library directory" FORCE)
set(SQLite_LIBRARIES ${PC_SQLITE_LIBRARIES} CACHE STRING "SQLite libraries" FORCE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite
                                  DEFAULT_MSG
                                  SQLite_LIBRARIES
)

set(SQLite_FOUND ${SQLITE_FOUND})

message(STATUS "SQLite_FOUND = ${SQLite_FOUND}")
message(STATUS "SQLite_INCLUDE_DIRS = ${SQLite_INCLUDE_DIRS}")
message(STATUS "SQLite_LIBRARY_DIRS = ${SQLite_LIBRARY_DIRS}")
message(STATUS "SQLite_LIBRARIES = ${SQLite_LIBRARIES}")

mark_as_advanced(SQLite_INCLUDE_DIRS SQLite_LIBRARY_DIRS SQLite_LIBRARIES)
