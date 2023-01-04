# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_catch2)

add_library(YARP_catch2 STATIC)
add_library(YARP::YARP_catch2 ALIAS YARP_catch2)

set(catch2_SRCS
  catch2/catch_amalgamated.cpp
)

set(catch2_HDRS
  catch2/catch_amalgamated.hpp
)

target_sources(YARP_catch2 PRIVATE ${catch2_SRCS})

target_include_directories(YARP_catch2 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/catch2>)

#This definition means that the default main from catch2 is disabled. User has to provide its own main.
target_compile_definitions(YARP_catch2 PUBLIC CATCH_AMALGAMATED_CUSTOM_MAIN)

set_property(TARGET YARP_catch2 PROPERTY FOLDER "Libraries/External")


#set(catch2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/catch2 PARENT_SCOPE)
#set(catch2_LIBRARIES YARP_catch2 PARENT_SCOPE)
#set(catch2_DEFINITIONS "" PARENT_SCOPE)

install(
  TARGETS YARP_catch2
  EXPORT YARP_catch2
  COMPONENT YARP_catch2
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

if(NOT CREATE_SHARED_LIBS)
  include(YarpInstallBasicPackageFiles)
  yarp_install_basic_package_files(YARP_catch2)
endif()
