# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# this is set in the parent directory, but for some reason it does not get inherited
set_source_files_properties(${SWIG_BINDINGS_SOURCE_FILE} PROPERTIES CPLUSPLUS ON)

set(CMAKE_SWIG_FLAGS "-module;yarp;${SWIG_COMMON_FLAGS}")
swig_add_library(yarp_brainfuck
                 LANGUAGE brainfuck
                 SOURCES ${SWIG_BINDINGS_SOURCE_FILE})

swig_link_libraries(yarp_brainfuck ${SWIG_YARP_LIBRARIES})

set_target_properties(${SWIG_MODULE_yarp_brainfuck_REAL_NAME} PROPERTIES OUTPUT_NAME "yarp_brainfuck")
