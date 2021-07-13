# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

set(yarpidl_thrift_SRCS
  "${THRIFT_ROOT}/compiler/cpp/src/thrift/version.h"
  "${THRIFT_ROOT}/compiler/cpp/src/thrift/common.cc"
  "${THRIFT_ROOT}/compiler/cpp/src/thrift/generate/t_generator.cc"
  "${THRIFT_ROOT}/compiler/cpp/src/thrift/parse/t_typedef.cc"
  "${THRIFT_ROOT}/compiler/cpp/src/thrift/parse/parse.cc"
  "${THRIFT_ROOT}/compiler/cpp/src/thrift/main.cc"
  "${THRIFT_ROOT}/compiler/cpp/src/thrift/audit/t_audit.cpp"
  "${THRIFT_ROOT}/compiler/cpp/src/thrift/generate/t_cpp_generator.cc"
)

set(yarpidl_thrift_YARP_SRCS src/t_yarp_generator.cc)


if(YARP_HAS_BISON AND YARP_HAS_FLEX AND NOT CMAKE_CROSSCOMPILING)
  bison_target(
    thrifty
    "${THRIFT_ROOT}/compiler/cpp/src/thrift/thrifty.yy"
    "${CMAKE_CURRENT_BINARY_DIR}/thrifty.cc"
    COMPILE_FLAGS --no-lines
  )
  flex_target(
    thriftl
    "${THRIFT_ROOT}/compiler/cpp/src/thrift/thriftl.ll"
    "${CMAKE_CURRENT_BINARY_DIR}/thriftl.cc"
    COMPILE_FLAGS --noline
  )
  add_flex_bison_dependency(thriftl thrifty)
else()
  # Simulate bison and flex run
  set(BISON_thrifty_DEFINED TRUE)
  set(BISON_thrifty_INPUT "${THRIFT_ROOT}/compiler/cpp/src/thrifty.yy")
  set(BISON_thrifty_OUTPUT_SOURCE "${THRIFT_ROOT}/compiler/cpp/src/thrift/thrifty.cc")
  set(BISON_thrifty_OUTPUT_HEADER "${THRIFT_ROOT}/compiler/cpp/src/thrift/thrifty.hh")
  set(BISON_thrifty_OUTPUTS "${BISON_thrifty_OUTPUT_SOURCE}" "${BISON_thrifty_OUTPUT_HEADER}")
  set(BISON_thrifty_COMPILE_FLAGS -d)
  set(FLEX_thriftl_DEFINED TRUE)
  set(FLEX_thriftl_OUTPUTS "${THRIFT_ROOT}/compiler/cpp/src/thrift/thriftl.cc")
  set(FLEX_thriftl_INPUT "${THRIFT_ROOT}/compiler/cpp/src/thriftl.ll")
  set(FLEX_thriftl_OUTPUT_HEADER "")
endif()

set(yarpidl_thrift_GEN_SRCS ${BISON_thrifty_OUTPUTS} ${FLEX_thriftl_OUTPUTS})

include_directories(SYSTEM ${THRIFT_ROOT}/compiler/cpp/src)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DYYENABLE_NLS=0)

if(MSVC)
  set_property(
    SOURCE ${yarpidl_thrift_SRCS}
           ${yarpidl_thrift_GEN_SRCS}
    APPEND_STRING
    PROPERTY COMPILE_FLAGS " /wd4003 /wd4005 /wd4065 /wd4068 /wd4102 /wd4244 "
  )
else()
  if(CXX_HAS_WREDUNDANT_DECLS)
    set_property(
      SOURCE ${yarpidl_thrift_SRCS}
             ${yarpidl_thrift_GEN_SRCS}
      APPEND_STRING
      PROPERTY COMPILE_FLAGS " -Wno-redundant-decls"
    )
  endif()
  if(CXX_HAS_WSUGGEST_OVERRIDE)
    set_property(
      SOURCE ${yarpidl_thrift_SRCS}
             ${yarpidl_thrift_GEN_SRCS}
      APPEND_STRING
      PROPERTY COMPILE_FLAGS " -Wno-suggest-override"
    )
  endif()
  if(CXX_HAS_WSIGN_COMPARE)
    set_property(
      SOURCE ${yarpidl_thrift_SRCS}
             ${yarpidl_thrift_GEN_SRCS}
      APPEND_STRING
      PROPERTY COMPILE_FLAGS " -Wno-sign-compare"
    )
  endif()
  if(CXX_HAS_WNULL_CONVERSION)
    set_property(
      SOURCE ${yarpidl_thrift_SRCS}
             ${yarpidl_thrift_GEN_SRCS}
      APPEND_STRING
      PROPERTY COMPILE_FLAGS " -Wno-null-conversion"
    )
  endif()

  # Building thrift with clang sanitizer will cause errors when running
  # thrift, and it will make the build fail since it is executed during
  # the build.
  # Therefore the '-fsanitize' build flag is just removed in order to
  # avoid build failures, and allow to test the rest of YARP with clang
  # sanitizers.
  if("${CMAKE_CXX_FLAGS}" MATCHES "(-fsanitize[^ ]+)" OR
     "${CMAKE_MODULE_LINKER_FLAGS}" MATCHES "(-fsanitize[^ ]+)" OR
     "${CMAKE_SHARED_LINKER_FLAGS}" MATCHES "(-fsanitize[^ ]+)" OR
     "${CMAKE_EXE_LINKER_FLAGS}" MATCHES "(-fsanitize[^ ]+)")

    message(WARNING "Sanitizer are not supported for thrift compiler. Disabling them")
    string(REGEX REPLACE "(-fsanitize[^ ]+)" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REGEX REPLACE "(-fsanitize[^ ]+)" "" CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS})
    string(REGEX REPLACE "(-fsanitize[^ ]+)" "" CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS})
    string(REGEX REPLACE "(-fsanitize[^ ]+)" "" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
  endif()

endif()

add_executable(yarpidl_thrift)
add_executable(YARP::yarpidl_thrift ALIAS yarpidl_thrift)

target_sources(yarpidl_thrift
  PRIVATE
    ${yarpidl_thrift_SRCS}
    ${yarpidl_thrift_GEN_SRCS}
    ${yarpidl_thrift_YARP_SRCS}
)

install(
  TARGETS yarpidl_thrift
  EXPORT YARP_idl_tools
  COMPONENT YARP_idl_tools
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_property(TARGET yarpidl_thrift PROPERTY FOLDER "Command Line Tools")


yarp_deprecated_option(THRIFT_USE_EXTERNAL) # Since YARP 3.0.0
yarp_deprecated_option(THRIFT_ROOT) # Since YARP 3.0.0
