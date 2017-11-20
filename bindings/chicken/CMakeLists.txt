# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Arjan Gijsberts, Lorenzo Natale, Fabien Benureau, Stephane Lallee, Ali Paikan, Francesco Romano
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# this is set in the parent directory, but for some reason it does not get inherited
set_source_files_properties(${SWIG_BINDINGS_SOURCE_FILE} PROPERTIES CPLUSPLUS ON)

set(CMAKE_SWIG_FLAGS "-module;cyarp;${SWIG_COMMON_FLAGS}")
swig_add_library(yarp_chicken
                 LANGUAGE chicken
                 SOURCES ${SWIG_BINDINGS_SOURCE_FILE})

swig_link_libraries(yarp_chicken ${SWIG_YARP_LIBRARIES})

set_target_properties(${SWIG_MODULE_yarp_chicken_REAL_NAME} PROPERTIES OUTPUT_NAME "libcyarp")
