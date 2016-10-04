# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(YARP_COMPILE_TESTS)

  # Add the CMAKE_BINARY_DIR as a macro so the testing infrastructure
  # can find the device compiled as dynamic plugins
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/YarpBuildLocation.h.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/YarpBuildLocation.h"
                 @ONLY)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})



  #### temporary variable for debug
  set(ENABLE_YARPRUNTESTS FALSE CACHE BOOL "Enable yarprun tests")
  if(ENABLE_YARPRUNTESTS)
    add_definitions(-DYARPRUN_TEST)
    add_executable(testModule ${CMAKE_SOURCE_DIR}/tests/libYARP_OS/testModule/module.cpp)
     target_link_libraries(testModule YARP_init YARP_OS)
  endif()
  mark_as_advanced(ENABLE_YARPRUNTESTS)

  set(targets OS
              sig
              dev
              serversql)
  get_property(YARP_HAS_MATH_LIB GLOBAL PROPERTY YARP_HAS_MATH_LIB)
  if(YARP_HAS_MATH_LIB)
    list(APPEND targets math)
  endif()
  if(TARGET YARP_wire_rep_utils)
    list(APPEND targets wire_rep_utils)
  endif()

  foreach(test_family ${targets})
    file(GLOB harness_code ${CMAKE_SOURCE_DIR}/tests/libYARP_${test_family}/*.cpp
                           ${CMAKE_SOURCE_DIR}/tests/libYARP_${test_family}/*.h)

    if("${test_family}" STREQUAL "dev" AND NOT YARP_HAS_MATH_LIB)
       # Without MATH lib, we don't compile FrameTransformClient
       list(REMOVE_ITEM harness_code ${CMAKE_CURRENT_SOURCE_DIR}/libYARP_dev/FrameTransformClientTest.cpp)
    endif()

    get_property(YARP_TREE_INCLUDE_DIRS
                 TARGET YARP_${test_family}
                 PROPERTY INCLUDE_DIRS)
    get_property(YARP_LIBRARIES
                 GLOBAL
                 PROPERTY YARP_LIBS)
    get_property(YARP_serversql_INCLUDE_DIRS
                 TARGET YARP_serversql
                 PROPERTY INCLUDE_DIRS)
    include_directories(${YARP_TREE_INCLUDE_DIRS})
    include_directories(${YARP_serversql_INCLUDE_DIRS})
    if(YARP_HAS_ACE)
      include_directories(${ACE_INCLUDE_DIRS})
    endif()

    set(EXE harness_${test_family})
    string(TOLOWER ${EXE} EXE)
    add_executable(${EXE} ${harness_code})
    target_link_libraries(${EXE} ${YARP_LIBRARIES})
    target_link_libraries(${EXE} YARP_serversql)
    if(YARP_HAS_ACE)
      target_link_libraries(${EXE} ${ACE_LIBRARIES})
    endif()

    foreach(test ${harness_code})
      get_filename_component(XN ${test} NAME_WE)
      if(NOT "${XN}" MATCHES "^(harness|TestList)$")
        set(_cmd $<TARGET_FILE:${EXE}> verbose regression ${XN})
        add_test(NAME "${test_family}::${XN}"
                 COMMAND ${_cmd})
        foreach(_Tool ${YARP_VALGRIND_TOOLS})
          string(TOUPPER "${_Tool}" _TOOL)
          if(YARP_VALGRIND_${_TOOL}_TESTS)
            add_test(NAME "${test_family}::${XN}::Valgrind::${_Tool}"
                     COMMAND ${VALGRIND_${_TOOL}_COMMAND} ${_cmd})
          endif()
        endforeach()
        if(YARP_GDB_TESTS)
          add_test(NAME "${test_family}::${XN}::gdb"
                   COMMAND ${GDB_COMMAND} ${_cmd})
        endif()
      endif()
    endforeach()
  endforeach()

  if(UNIX)
    target_link_libraries(harness_os pthread)
  endif()

  # Compile FrameTransformClientTest  only if yarp math is built
  if (YARP_HAS_MATH_LIB)
    add_definitions(-DWITH_YARPMATH)
  endif()

  # Compile ControlBoardRemapperTest only if fakeMotionControl is enabled
  if(ENABLE_yarpmod_fakeMotionControl)
    add_definitions(-DYARP_CONTROLBOARDREMAPPER_TESTS)
  endif()

  # Add in hardware specific tests, if requested
  option(CREATE_PLUGIN_TESTS "Compile plugin tests" FALSE)
  mark_as_advanced(CREATE_PLUGIN_TESTS)
  if(CREATE_PLUGIN_TESTS)
    # add in our tests
    file(GLOB inis ${CMAKE_SOURCE_DIR}/src/libYARP_dev/harness/*.ini)
    foreach(X ${inis})
      get_filename_component(XN ${X} NAME_WE)
      get_filename_component(XINI ${X} ABSOLUTE)
      # need to cache examples for reference
      set(DOC_DIR "${CMAKE_SOURCE_DIR}/doc/device_invocation")
      set(XDOX ${DOC_DIR}/${XN}.dox)
      set(_cmd $<TARGET_FILE:harness_dev> --file "${XINI}" --doc "${XDOX}")
      add_test(NAME "device::${XN}"
               COMMAND ${_cmd}
               WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/libYARP_dev/harness)
      foreach(_Tool ${YARP_VALGRIND_TOOLS})
        string(TOUPPER "${_Tool}" _TOOL)
        if(YARP_VALGRIND_${_TOOL}_TESTS)
          add_test(NAME "device::${XN}::Valgrind::${_Tool}"
                   COMMAND ${VALGRIND_MEMCHECK_COMMAND} ${_cmd}
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/libYARP_dev/harness)
        endif()
      endforeach()
      if(YARP_GDB_TESTS)
        add_test(NAME "device::${XN}::gdb"
                 COMMAND ${GDB_COMMAND} ${_cmd}
                 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src/libYARP_dev/harness)
      endif()
    endforeach(X ${inis})
  endif()

  # Integration tests
  option(YARP_TEST_INTEGRATION "Run integration tests (UNIX only)" FALSE)
  mark_as_advanced(YARP_TEST_INTEGRATION)
  if(YARP_TEST_INTEGRATION)
    add_subdirectory(integration)
  endif()
endif()
