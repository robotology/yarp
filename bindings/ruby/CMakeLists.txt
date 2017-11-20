# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Arjan Gijsberts, Lorenzo Natale, Fabien Benureau, Stephane Lallee, Ali Paikan, Francesco Romano
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# this is set in the parent directory, but for some reason it does not get inherited
set_source_files_properties(${SWIG_BINDINGS_SOURCE_FILE} PROPERTIES CPLUSPLUS ON)

find_package(Ruby)
set(CMAKE_SWIG_FLAGS "-module;yarp;${SWIG_COMMON_FLAGS}")


swig_add_library(yarp_ruby
                 LANGUAGE ruby
                 SOURCES ${SWIG_BINDINGS_SOURCE_FILE})
swig_link_libraries(yarp_ruby ${RUBY_LIBRARY} ${SWIG_YARP_LIBRARIES})
target_include_directories(${SWIG_MODULE_yarp_ruby_REAL_NAME} SYSTEM PRIVATE ${RUBY_INCLUDE_PATH})

set_target_properties(${SWIG_MODULE_yarp_ruby_REAL_NAME} PROPERTIES PREFIX ""
                                                                    OUTPUT_NAME "yarp")

if(NOT MSVC)
  check_cxx_compiler_flag("-Wno-sign-compare" CXX_HAS_WNO_SIGN_COMPARE)
  if(CXX_HAS_WNO_SIGN_COMPARE)
    target_compile_options(${SWIG_MODULE_yarp_ruby_REAL_NAME} PRIVATE "-Wno-sign-compare")
  endif()
endif()

if(YARP_COMPILE_TESTS)
  add_subdirectory(tests)
endif()
