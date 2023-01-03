# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# SQLite
project(YARP_priv_sqlite3)

add_library(YARP_priv_sqlite3 STATIC)

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

target_sources(YARP_priv_sqlite3 PRIVATE ${sqlite_SRCS})

target_include_directories(YARP_priv_sqlite3 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sqlite>)

target_compile_definitions(YARP_priv_sqlite3 PRIVATE SQLITE_OMIT_LOAD_EXTENSION)

if(UNIX)
  target_link_libraries(YARP_priv_sqlite3 PRIVATE pthread)
endif(UNIX)

set_property(TARGET YARP_priv_sqlite3 PROPERTY FOLDER "Libraries/External")


set(SQLite3_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/sqlite PARENT_SCOPE)
set(SQLite3_LIBRARIES YARP_priv_sqlite3 PARENT_SCOPE)
set(SQLite3_DEFINITIONS "" PARENT_SCOPE)

install(
  TARGETS YARP_priv_sqlite3
  EXPORT YARP_priv_sqlite3
  COMPONENT YARP_priv_sqlite3
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

if(NOT CREATE_SHARED_LIBS)
  include(YarpInstallBasicPackageFiles)
  yarp_install_basic_package_files(YARP_priv_sqlite3)
endif()
