# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# this is set in the parent directory, but for some reason it does not get inherited
set_source_files_properties(${SWIG_BINDINGS_SOURCE_FILE} PROPERTIES CPLUSPLUS ON)


set(CMAKE_SWIG_FLAGS "${SWIG_COMMON_FLAGS}")

if(WIN32 AND NOT CYGWIN)
  option(PREPARE_CLASS_FILES "Prepare NET library" ON)
  swig_add_library(yarp_csharp
                   LANGUAGE csharp
                   SOURCES ${SWIG_BINDINGS_SOURCE_FILE})

  if(PREPARE_CLASS_FILES)
    add_custom_command(TARGET ${SWIG_MODULE_yarp_csharp_REAL_NAME}
                       POST_BUILD
                       COMMAND IF NOT EXIST .\\generated_src mkdir generated_src
                       WORKING_DIRECTORY ${YARP_BASE_DIR})

    add_custom_command(TARGET ${SWIG_MODULE_yarp_csharp_REAL_NAME}
                       POST_BUILD
                       COMMAND move *.cs ${YARP_BASE_DIR}/generated_src
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  endif()
else(WIN32 AND NOT CYGWIN)
  swig_add_library(yarp_csharp
                   LANGUAGE csharp
                   SOURCES ${SWIG_BINDINGS_SOURCE_FILE})
endif(WIN32 AND NOT CYGWIN)

swig_link_libraries(yarp_csharp ${SWIG_YARP_LIBRARIES})

set_target_properties(${SWIG_MODULE_yarp_csharp_REAL_NAME} PROPERTIES OUTPUT_NAME "yarp")

if(YARP_COMPILE_TESTS)
  add_subdirectory(tests)
endif()
