# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(YARP_rosmsg)

include(YarpChooseIDL)

# yarpRosHelper.h is used by several devices
# For now in impl since it might be removed in the future
set(YARP_rosmsgs_IMPL_HDRS yarp/rosmsg/impl/yarpRosHelper.h)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}"
  PREFIX "Header Files"
  FILES ${YARP_rosmsgs_IMPL_HDRS}
)


# Add main target
add_custom_target(YARP_rosmsg_gen_all ALL)
set_property(TARGET YARP_rosmsg_gen_all PROPERTY FOLDER "Libraries/YARP_rosmsg")

# Add main library
add_library(YARP_rosmsg INTERFACE)
add_library(YARP::YARP_rosmsg ALIAS YARP_rosmsg)
target_include_directories(YARP_rosmsg
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(YARP_rosmsg INTERFACE cxx_std_14)
target_link_libraries(YARP_rosmsg INTERFACE YARP::YARP_os)
list(APPEND YARP_rosmsg_PUBLIC_DEPS YARP_os)

add_dependencies(YARP_rosmsg YARP_rosmsg_gen_all)


# ROS native types
yarp_choose_idl(
  native_MSG
  time
  duration
)

add_custom_target(YARP_rosmsg_gen_native
  DEPENDS
    ${native_MSG_GEN_FILES}
)
set_property(TARGET YARP_rosmsg_gen_native PROPERTY FOLDER "Libraries/YARP_rosmsg")

add_library(YARP_rosmsg_native INTERFACE)
add_library(YARP::YARP_rosmsg_native ALIAS YARP_rosmsg_native)
target_include_directories(YARP_rosmsg_native
  INTERFACE
    $<BUILD_INTERFACE:${native_MSG_BUILD_INTERFACE_INCLUDE_DIRS}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(YARP_rosmsg_native INTERFACE cxx_std_14)
target_link_libraries(YARP_rosmsg_native INTERFACE YARP::YARP_os)
list(APPEND YARP_rosmsg_native_PUBLIC_DEPS YARP_os)

add_dependencies(YARP_rosmsg_gen_all YARP_rosmsg_gen_native)
target_link_libraries(YARP_rosmsg INTERFACE YARP::YARP_rosmsg_native)
list(APPEND YARP_rosmsg_PUBLIC_DEPS YARP_rosmsg_native)

# Deprecated files are not installed since they were never installed
# YARP_NO_DEPRECATED since YARP v3.0.0
unset(native_INSTALL_FILES)
foreach(_file ${native_MSG_GEN_FILES})
  if("${_file}" MATCHES "yarp/rosmsg/")
    list(APPEND native_INSTALL_FILES "${_file}")
  endif()
endforeach()



# Other ROS packages
foreach(_pkg ${ROS_MSG_PACKAGES})
  # Force CMake to run again if package.xml is updated, i.e. we included a new
  # version of the package.
  # This makes it safe to use file(GLOB)
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${ROS_${_pkg}_ROOT}/package.xml")

  # Generate list of .msg and .srv files
  file(GLOB ${_pkg}_MSG_FILES "${ROS_${_pkg}_ROOT}/msg/*.msg")
  file(GLOB ${_pkg}_SRV_FILES "${ROS_${_pkg}_ROOT}/srv/*.srv")

  source_group(
    TREE "${ROS_${_pkg}_ROOT}"
    PREFIX "${_pkg} .msg Files"
    FILES ${${_pkg}_MSG_FILES}
  )
  source_group(
    TREE "${ROS_${_pkg}_ROOT}"
    PREFIX "${_pkg} .srv Files"
    FILES ${${_pkg}_SRV_FILES}
  )

  yarp_choose_idl(${_pkg}_MSG ${${_pkg}_MSG_FILES})
  yarp_choose_idl(${_pkg}_SRV ${${_pkg}_SRV_FILES})

  # Add custom target and set it as YARP_rosmsg_gen_all dependency
  add_custom_target(YARP_rosmsg_gen_${_pkg}
    DEPENDS
      ${${_pkg}_MSG_GEN_FILES}
      ${${_pkg}_SRV_GEN_FILES}
  )
  set_property(TARGET YARP_rosmsg_gen_${_pkg} PROPERTY FOLDER "Libraries/YARP_rosmsg")
  add_dependencies(YARP_rosmsg_gen_all YARP_rosmsg_gen_${_pkg})

  # Add library
  add_library(YARP_rosmsg_${_pkg} INTERFACE)
  add_library(YARP::YARP_rosmsg_${_pkg} ALIAS YARP_rosmsg_${_pkg})
  target_include_directories(YARP_rosmsg_${_pkg}
    INTERFACE
      $<BUILD_INTERFACE:${${_pkg}_MSG_BUILD_INTERFACE_INCLUDE_DIRS}>
      $<BUILD_INTERFACE:${${_pkg}_SRV_BUILD_INTERFACE_INCLUDE_DIRS}>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
  target_compile_features(YARP_rosmsg_${_pkg} INTERFACE cxx_std_14)
  target_link_libraries(YARP_rosmsg_${_pkg}
    INTERFACE
      YARP::YARP_os
      YARP::YARP_rosmsg_native
  )
  list(APPEND YARP_rosmsg_${_pkg}_PUBLIC_DEPS
    YARP_os
    YARP_rosmsg_native
  )

  # YARP_rosmsg depends on all the packages
  add_dependencies(YARP_rosmsg YARP_rosmsg_gen_${_pkg})
  target_link_libraries(YARP_rosmsg
    INTERFACE
      YARP::YARP_rosmsg_${_pkg}
  )
  list(APPEND YARP_rosmsg_PUBLIC_DEPS
    YARP_rosmsg_${_pkg}
  )

  # Install files and exports
  # Deprecated files are not installed since they were never installed
  # YARP_NO_DEPRECATED since YARP v3.0.0
  unset(${_pkg}_INSTALL_FILES)
  foreach(_file
    ${${_pkg}_MSG_GEN_FILES}
    ${${_pkg}_SRV_GEN_FILES}
  )
    if("${_file}" MATCHES "yarp/rosmsg/${_pkg}/")
      list(APPEND ${_pkg}_INSTALL_FILES "${_file}")
    endif()
  endforeach()
endforeach()

# Handle dependencies after adding all packages so that we can ignore the
# actual order when adding the libraries
foreach(_pkg ${ROS_MSG_PACKAGES})
  file(STRINGS "${ROS_${_pkg}_ROOT}/package.xml" _content REGEX "build_depend")
  if(_content)
    foreach(_line ${_content})
      string(REGEX REPLACE "^.*<build_depend.*>(.+)</build_depend>.*" "\\1" _dep "${_line}")
      if(TARGET YARP::YARP_rosmsg_${_dep})
        target_link_libraries(YARP_rosmsg_${_pkg} INTERFACE YARP::YARP_rosmsg_${_dep})
        list(APPEND YARP_rosmsg_${_pkg}_PUBLIC_DEPS YARP_rosmsg_${_dep})
      endif()
    endforeach()
  endif()
endforeach()


# Set library properties
# set_property(TARGET YARP_rosmsg_native PROPERTY PUBLIC_HEADER ${YARP_rosmsg_native_HDRS})
# set_property(TARGET YARP_rosmsg_native PROPERTY PRIVATE_HEADER ${YARP_rosmsg_native_IMPL_HDRS})
# set_property(TARGET YARP_rosmsg_native PROPERTY VERSION ${YARP_VERSION_SHORT})
# set_property(TARGET YARP_rosmsg_native PROPERTY SOVERSION ${YARP_SOVERSION})
# set_property(TARGET YARP_rosmsg_native PROPERTY FOLDER "Libraries")
# foreach(_pkg ${ROS_MSG_PACKAGES})
#   set_property(TARGET YARP_rosmsg_${_pkg} PROPERTY PUBLIC_HEADER ${YARP_rosmsg_${_pkg}_HDRS})
#   set_property(TARGET YARP_rosmsg_${_pkg} PROPERTY PRIVATE_HEADER ${YARP_rosmsg_${_pkg}_IMPL_HDRS})
#   set_property(TARGET YARP_rosmsg_${_pkg} PROPERTY VERSION ${YARP_VERSION_SHORT})
#   set_property(TARGET YARP_rosmsg_${_pkg} PROPERTY SOVERSION ${YARP_SOVERSION})
#   set_property(TARGET YARP_rosmsg_${_pkg} PROPERTY FOLDER "Libraries")
# endforeach()
# set_property(TARGET YARP_rosmsg PROPERTY PUBLIC_HEADER ${YARP_rosmsg_HDRS})
# set_property(TARGET YARP_rosmsg PROPERTY PRIVATE_HEADER ${YARP_rosmsg_IMPL_HDRS})
# set_property(TARGET YARP_rosmsg PROPERTY VERSION ${YARP_VERSION_SHORT})
# set_property(TARGET YARP_rosmsg PROPERTY SOVERSION ${YARP_SOVERSION})
# set_property(TARGET YARP_rosmsg PROPERTY FOLDER "Libraries")


# Install all libraries
# INTERFACE libraries do not support the PUBLIC_HEADER property
# https://gitlab.kitware.com/cmake/cmake/issues/20056
install(
  TARGETS YARP_rosmsg_native
  EXPORT YARP_rosmsg_native
)
install(
  FILES ${native_INSTALL_FILES}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/rosmsg
  COMPONENT YARP_rosmsg_native-dev
)

foreach(_pkg ${ROS_MSG_PACKAGES})
  install(
    TARGETS YARP_rosmsg_${_pkg}
    EXPORT YARP_rosmsg_${_pkg}
  )
  install(
    FILES ${${_pkg}_INSTALL_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/rosmsg/${_pkg}
    COMPONENT YARP_rosmsg_${_pkg}-dev
  )
endforeach()

install(
  TARGETS YARP_rosmsg
  EXPORT YARP_rosmsg
)
install(
  FILES ${YARP_rosmsgs_IMPL_HDRS}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/rosmsg/impl
  COMPONENT YARP_rosmsg-dev
)


# Set variables for parent scope
set(YARP_rosmsg_native_PUBLIC_DEPS ${YARP_rosmsg_native_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_rosmsg_native_PRIVATE_DEPS ${YARP_rosmsg_native_PRIVATE_DEPS} PARENT_SCOPE)
foreach(_pkg ${ROS_MSG_PACKAGES})
  set(YARP_rosmsg_${_pkg}_PUBLIC_DEPS ${YARP_rosmsg_${_pkg}_PUBLIC_DEPS} PARENT_SCOPE)
  set(YARP_rosmsg_${_pkg}_PRIVATE_DEPS ${YARP_rosmsg_${_pkg}_PRIVATE_DEPS} PARENT_SCOPE)
endforeach()
set(YARP_rosmsg_PUBLIC_DEPS ${YARP_rosmsg_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_rosmsg_PRIVATE_DEPS ${YARP_rosmsg_PRIVATE_DEPS} PARENT_SCOPE)
