# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

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

if(MSVC)
    set_source_files_properties(sqlite/sqlite3.c PROPERTIES COMPILE_FLAGS "/wd4996")
endif()

add_definitions(-DSQLITE_OMIT_LOAD_EXTENSION)

add_library(YARP_priv_sqlite STATIC ${sqlite_SRCS})

set_property(TARGET YARP_priv_sqlite PROPERTY FOLDER "Libraries/External")

if(UNIX)
    target_link_libraries(YARP_priv_sqlite PRIVATE pthread)
endif(UNIX)

set(SQLite_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/sqlite PARENT_SCOPE)
set(SQLite_LIBRARIES YARP_priv_sqlite PARENT_SCOPE)
set(SQLite_DEFINITIONS "" PARENT_SCOPE)

install(TARGETS YARP_priv_sqlite
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
