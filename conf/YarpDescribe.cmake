# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Let's see what we built, and record it to facilitate in-tree 
# ("uninstalled") use of YARP.
get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
get_property(YARP_LINK_DIRS GLOBAL PROPERTY YARP_TREE_LINK_DIRS)
get_property(YARP_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
# Oops, cannot use YARP_DEFINES name, conflicts with an old variable
# that might be lurking in CMakeCache.txt as people upgrade.  Insert 
# an "_ALL_" for now.
get_property(YARP_ALL_DEFINES GLOBAL PROPERTY YARP_DEFS)
get_property(YARP_HAS_MATH_LIB GLOBAL PROPERTY YARP_HAS_MATH_LIB)
message(STATUS "In-tree includes: ${YARP_INCLUDE_DIRS}")
message(STATUS "YARP libraries: ${YARP_LIBRARIES}")

set(YARP_DEPENDENCY_FILE ${CMAKE_BINARY_DIR}/YARPDependencies.cmake)
set(YARP_DEPENDENCY_FILENAME YARPDependencies.cmake)
configure_file(${CMAKE_SOURCE_DIR}/conf/template/YARPConfig.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfig.cmake @ONLY IMMEDIATE)
configure_file(${CMAKE_SOURCE_DIR}/conf/template/YARPConfigVersion.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfigVersion.cmake @ONLY IMMEDIATE)
export(TARGETS ${YARP_LIBRARIES} FILE ${YARP_DEPENDENCY_FILE})

set(VERSIONED_LIB lib${LIB_SUFFIX}/YARP-${YARP_GENERIC_VERSION})

# Set up a configuration file for installed use of YARP
set(YARP_DEPENDENCY_FILE ${CMAKE_INSTALL_PREFIX}/${VERSIONED_LIB}/YARP.cmake)
set(YARP_DEPENDENCY_FILENAME YARP.cmake)
set(YARP_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX}/include)
set(YARP_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/share/yarp/cmake)
configure_file(${CMAKE_SOURCE_DIR}/conf/template/YARPConfig.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfigForInstall.cmake @ONLY IMMEDIATE)
install(FILES ${CMAKE_BINARY_DIR}/YARPConfigForInstall.cmake RENAME YARPConfig.cmake COMPONENT configuration DESTINATION ${VERSIONED_LIB})
install(FILES ${CMAKE_BINARY_DIR}/YARPConfigVersion.cmake COMPONENT configuration DESTINATION ${VERSIONED_LIB})
install(EXPORT YARP COMPONENT configuration DESTINATION ${VERSIONED_LIB})

foreach(lib ${YARP_LIBRARIES})
  set_target_properties(${lib} PROPERTIES VERSION ${YARP_GENERIC_VERSION}
	SOVERSION ${YARP_GENERIC_SOVERSION})
endforeach()


