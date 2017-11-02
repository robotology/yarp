# Copyright: (C) 2012 Istituto Italiano di Tecnologia (IIT)
# Authors: Elena Ceseracciu
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(generatedDemoLibrary)

set(THRIFT_HEADERS)
set(THRIFT_SOURCES)
set(fileList firstInterface_PointD_thrift.cmake
             secondInterface_demo_thrift.cmake)
foreach(scriptFile ${fileList})
  include(${scriptFile})
  list(APPEND THRIFT_HEADERS ${headers})
  list(APPEND THRIFT_SOURCES ${sources})
endforeach()
message("Thrift headers ${THRIFT_HEADERS}.\n Thirft sources: ${THRIFT_SOURCES}")
set(generated_libs_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "Generated-files libraries include directory")
include_directories(${generated_libs_include_dir})

add_library(generatedDemoLibrary ${THRIFT_HEADERS} ${THRIFT_SOURCES})
target_link_libraries(generatedDemoLibrary ${YARP_LIBRARIES})
set_target_properties(generatedDemoLibrary PROPERTIES LINKER_LANGUAGE CXX)
