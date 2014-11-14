# Copyright: (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

find_path(THRIFT_ROOT compiler/cpp/src/thrifty.yy PATHS ${CMAKE_SOURCE_DIR}/thrift ${CMAKE_SOURCE_DIR}/../thrift ENV THRIFT_ROOT DOC "Location of Thrift source code repository (optional)")
option(THRIFT_USE_EXTERNAL "Use external Thrift source" FALSE)
mark_as_advanced(THRIFT_ROOT THRIFT_USE_EXTERNAL)

# Use external version of thrift, patching tcc to support yarp
if (THRIFT_USE_EXTERNAL)
  set(THRIFT_COMPILER_SOURCE ${THRIFT_ROOT}/compiler/cpp)

  file(GLOB_RECURSE thrift_source_cc ${THRIFT_COMPILER_SOURCE}/src/*.cc)
  file(GLOB_RECURSE thrift_source_c ${THRIFT_COMPILER_SOURCE}/src/*.c)

  set(tmp_src ${thrift_source_cc})
  set(thrift_source_cc)
  foreach(f ${tmp_src}) 
    string(REGEX MATCH "t_([a-z0-9_]+)_generator[.]cc$" isgen ${f})
    if (NOT isgen)
      list(APPEND thrift_source_cc ${f})
    endif ()
  endforeach ()
  file(GLOB_RECURSE insert_source_cc src/*.cc)

  find_package(BISON REQUIRED)
  find_package(FLEX REQUIRED)

  include_directories(${THRIFT_COMPILER_SOURCE})
  include_directories(${THRIFT_COMPILER_SOURCE}/src)
  include_directories(${THRIFT_COMPILER_SOURCE}/src/generate)
  bison_target(tcc_parser ${THRIFT_COMPILER_SOURCE}/src/thrifty.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cc)
  flex_target(tcc_scanner ${THRIFT_COMPILER_SOURCE}/src/thriftl.ll  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc)
  add_flex_bison_dependency(tcc_scanner tcc_parser)

  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})

  add_executable(yarpidl_thrift
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc
    ${thrift_source_cc}
    ${thrift_source_c}
    ${insert_source_cc}
    )
  install(TARGETS yarpidl_thrift COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()

if (NOT THRIFT_USE_EXTERNAL)
  # Use built-in version of thrift, pre flex/bisoned for min. dependencies.

  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src_gen)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src_thrift)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src_thrift/generate)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src_thrift/parse)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
  if (MSVC)
    add_definitions(-DYY_NO_UNISTD_H -Dstrtoll=_strtoi64 -DPRIi64="I64i")
    add_definitions(-DMINGW) # activate windows code in thrift
    add_definitions(-DPATH_MAX=1000 -DS_ISDIR=1+0*)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/msvc)
    add_definitions(-D__STDC_VERSION__=199901L)
  endif ()
  add_executable(yarpidl_thrift
    src_gen/parser.cc
    src_gen/lexer.cc
    src_thrift/md5.c
    src_thrift/main.cc
    src_thrift/generate/t_generator.cc
    src_thrift/parse/parse.cc
    src/t_yarp_generator
    )

endif ()

install(TARGETS yarpidl_thrift
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

option(TEST_yarpidl_thrift "Test thrift-based IDL" OFF)
if (TEST_yarpidl_thrift)
add_subdirectory(tests)
endif ()
