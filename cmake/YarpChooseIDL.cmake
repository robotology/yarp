# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

include(YarpIDL)

# NOTE: This is an internal module and it is not intended to be used outside
#       YARP.
# * yarpidl_rosmsg depends on YARP_init. This means that on static builds the
#   tool for re-generating the files depends on the generated files (yes, this
#   is madness). For now the only way to fix this is to use pre-generated files.
# * For cross-compiling, generating the files at build time could be an issue,
#   hence use pre-generated files.
# * Finally if ALLOW_IDL_GENERATION is enabled, we want to re-generate the
#   pre-generated files.
macro(YARP_CHOOSE_IDL _prefix)
  unset(${_prefix}_GEN_FILES)
  if(NOT "${ARGN}" STREQUAL "")
    if(BUILD_SHARED_LIBS AND
       NOT CMAKE_CROSSCOMPILING AND
       NOT ALLOW_IDL_GENERATION AND
       YARP_COMPILE_EXECUTABLES)
      set(YARP_ADD_IDL_THRIFT_INCLUDE_PREFIX 0)
      set(YARP_ADD_IDL_THRIFT_NO_NAMESPACE_PREFIX 0)
      yarp_add_idl(${_prefix}_GEN_FILES
                   ${ARGN})
      set(${_prefix}_BUILD_INTERFACE_INCLUDE_DIRS "${YARP_ADD_IDL_INCLUDE_DIR}")
    else()
      yarp_idl_to_dir(INPUT_FILES ${ARGN}
                      OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/idl_generated_code"
                      SOURCES_VAR ${_prefix}_GEN_SRCS
                      HEADERS_VAR ${_prefix}_GEN_HDRS
                      INCLUDE_DIRS_VAR ${_prefix}_BUILD_INTERFACE_INCLUDE_DIRS
                      PLACEMENT MERGED)
      set(${_prefix}_GEN_FILES ${${_prefix}_GEN_SRCS} ${${_prefix}_GEN_HDRS})
    endif()
  endif()
endmacro()
