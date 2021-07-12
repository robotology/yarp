# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# this is set in the parent directory, but for some reason it does not get inherited
set_source_files_properties(${SWIG_BINDINGS_SOURCE_FILE} PROPERTIES CPLUSPLUS ON)

set(CMAKE_SWIG_FLAGS "-module;yarp;${SWIG_COMMON_FLAGS}")
find_package(Octave REQUIRED)

get_filename_component(OCTAVE_INCLUDE_TOP ${OCTAVE_INCLUDE_DIR} DIRECTORY)

swig_add_library(yarp_octave
                 LANGUAGE octave
                 SOURCES ${SWIG_BINDINGS_SOURCE_FILE})

target_include_directories(${SWIG_MODULE_yarp_octave_REAL_NAME} SYSTEM PRIVATE ${OCTAVE_INCLUDE_DIRS} ${OCTAVE_INCLUDE_TOP})
swig_link_libraries(yarp_octave ${OCTAVE_LIBRARIES} ${SWIG_YARP_LIBRARIES})

set_target_properties(${SWIG_MODULE_yarp_octave_REAL_NAME} PROPERTIES PREFIX ""
                                                                      OUTPUT_NAME "yarp"
                                                                      SUFFIX ".oct"
                                                                      LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/octave)
