# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Let's see what we built, and record it to facilitate in-tree
# ("uninstalled") use of YARP.
get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
get_property(YARP_LINK_DIRS GLOBAL PROPERTY YARP_TREE_LINK_DIRS)
get_property(YARP_LIBS GLOBAL PROPERTY YARP_LIBS)
get_property(YARP_TOOLS GLOBAL PROPERTY YARP_TOOLS)

# Oops, cannot use YARP_DEFINES name, conflicts with an old variable
# that might be lurking in CMakeCache.txt as people upgrade.  Insert
# an "_ALL_" for now.
get_property(YARP_ALL_DEFINES GLOBAL PROPERTY YARP_DEFS)
get_property(YARP_HAS_MATH_LIB GLOBAL PROPERTY YARP_HAS_MATH_LIB)

message(STATUS "In-tree includes: ${YARP_INCLUDE_DIRS}")
message(STATUS "YARP libraries: ${YARP_LIBS}")

set(YARP_HAS_IDL TRUE)
set(YARP_IDL_BINARY_HINT ${CMAKE_BINARY_DIR}/bin)
if (MSVC)
    set(YARP_IDL_BINARY_HINT "${YARP_IDL_BINARY_HINT};${YARP_IDL_BINARY_HINT}/Debug;${YARP_IDL_BINARY_HINT}/Release")
endif(MSVC)

set(YARP_BINDINGS ${CMAKE_SOURCE_DIR}/bindings)

# Filter out from YARP_LIBRARIES all the plugins, yarpmod, yarpcar
# and all the other libraries that should not be linked by the user
set(YARP_LIBRARIES)
foreach(lib ${YARP_LIBS})
  if(NOT "${lib}" MATCHES "carrier$" AND
     NOT "${lib}" MATCHES "^yarp_" AND
     NOT "${lib}" MATCHES "^YARP_priv" AND
     NOT "${lib}" STREQUAL "yarpcar" AND
     NOT "${lib}" STREQUAL "yarpmod" AND
     NOT "${lib}" STREQUAL "YARP_wire_rep_utils" AND
     NOT "${lib}" STREQUAL "YARP_manager")
    list(APPEND YARP_LIBRARIES YARP::${lib})
  endif()
endforeach()

configure_file(${CMAKE_CURRENT_LIST_DIR}/template/YARPConfig.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfig.cmake @ONLY)
write_basic_package_version_file(${CMAKE_BINARY_DIR}/YARPConfigVersion.cmake
                                 VERSION ${YARP_VERSION}
                                 COMPATIBILITY AnyNewerVersion)

# YARPTargets.cmake (build tree)
export(TARGETS ${YARP_LIBS} ${YARP_TOOLS}
       NAMESPACE YARP::
       FILE ${CMAKE_BINARY_DIR}/YARPTargets.cmake)

set(YARP_CMAKE_DESTINATION ${CMAKE_INSTALL_LIBDIR}/YARP)

# Set up a configuration file for installed use of YARP
set(YARP_INCLUDE_DIRS ${CMAKE_INSTALL_FULL_INCLUDEDIR})
set(YARP_MODULE_DIR ${CMAKE_INSTALL_FULL_DATADIR}/yarp/cmake)
set(YARP_IDL_BINARY_HINT ${CMAKE_INSTALL_FULL_BINDIR})
set(YARP_BINDINGS ${CMAKE_INSTALL_FULL_PREFIX_DATADIR}/yarp/bindings)

set(YARP_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

configure_file(${CMAKE_CURRENT_LIST_DIR}/template/YARPConfig.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfigForInstall.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/YARPConfigForInstall.cmake RENAME YARPConfig.cmake COMPONENT configuration DESTINATION ${YARP_CMAKE_DESTINATION})
install(FILES ${CMAKE_BINARY_DIR}/YARPConfigVersion.cmake COMPONENT configuration DESTINATION ${YARP_CMAKE_DESTINATION})

# YARPTargets.cmake (installed)
install(EXPORT YARP
        NAMESPACE YARP::
        DESTINATION ${YARP_CMAKE_DESTINATION}
        FILE YARPTargets.cmake)

foreach(lib ${YARP_LIBS})
    set_target_properties(${lib} PROPERTIES VERSION ${YARP_VERSION}
                                            SOVERSION ${YARP_GENERIC_SOVERSION})

    # Compile libraries using -fPIC to produce position independent code
    # For CMAKE_VERSION >= 2.8.10 this is handled in YarpOptions.cmake
    # using the CMAKE_POSITION_INDEPENDENT_CODE flag
    if(${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_GREATER 2.8.9)
        message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
    endif()
    if(CMAKE_VERSION VERSION_EQUAL "2.8.9")
        set_target_properties(${lib} PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
    endif()
endforeach(lib)
