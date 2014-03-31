# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# SQLite
project(YARP_priv_sqlite)

set(sqlite_SRCS
    sqlite/shell.c
    sqlite/sqlite3.c
)

set(sqlite_HDRS
    sqlite/sqlite3ext.h
    sqlite/sqlite3.h
)

add_library(YARP_priv_sqlite STATIC ${sqlite_SRCS})

if(UNIX)
    set_target_properties(YARP_priv_sqlite PROPERTIES COMPILE_FLAGS -fPIC)
    target_link_libraries(YARP_priv_sqlite LINK_PRIVATE pthread)
endif(UNIX)

set(SQLite_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/sqlite PARENT_SCOPE)
set(SQLite_LIBRARIES YARP_priv_sqlite PARENT_SCOPE)
set(SQLite_DEFINITIONS "" PARENT_SCOPE)

mark_as_advanced(SQLite_INCLUDE_DIRS SQLite_LIBRARIES SQLite_DEFINITIONS)
