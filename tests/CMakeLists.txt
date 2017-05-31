# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


include(CMakeDependentOption)
cmake_dependent_option(YARP_ENABLE_BROKEN_TESTS OFF "Enable broken tests" YARP_COMPILE_TESTS OFF)
mark_as_advanced(YARP_ENABLE_BROKEN_TESTS)
if(YARP_ENABLE_BROKEN_TESTS)
  add_definitions(-DBROKEN_TEST)
endif()


if(YARP_COMPILE_TESTS)
  # Prepare 'build_generator', 'build_options' and 'build_module_path' variables
  # for tests in subdirectories
  set(build_generator --build-generator "${CMAKE_GENERATOR}")
  if(CMAKE_GENERATOR_PLATFORM)
    list(APPEND build_generator --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}")
  endif()
  if(CMAKE_GENERATOR_TOOLSET)
    list(APPEND build_generator --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}")
  endif()
  if(CMAKE_CONFIGURATION_TYPES)
    list(APPEND build_generator --build-config $<CONFIGURATION>)
  endif()

  unset(build_options)
  if(CMAKE_BUILD_TYPE)
    list(APPEND build_options -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
  endif()

  # Use only if cmake modules not installed by YARP are required by the test
  set(build_module_path -DCMAKE_MODULE_PATH="${CMAKE_MODULE_PATH}")
  string(REPLACE ";" "\;" build_module_path "${build_module_path}")

  # Add the CMAKE_BINARY_DIR as a macro so the testing infrastructure
  # can find the device compiled as dynamic plugins
  file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}" CMAKE_NATIVE_BINARY_DIR)
  file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/conf" TEST_DATA_DIR)
  if(CMAKE_HOST_WIN32)
    string(REPLACE "\\" "\\\\" CMAKE_NATIVE_BINARY_DIR "${CMAKE_NATIVE_BINARY_DIR}")
    string(REPLACE "\\" "\\\\" TEST_DATA_DIR "${TEST_DATA_DIR}")
  endif()

  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/YarpBuildLocation.h.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/YarpBuildLocation.h"
                 @ONLY)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})

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
       # Without MATH lib, we don't compile FrameTransformClient and MapGrid2D
       list(REMOVE_ITEM harness_code ${CMAKE_CURRENT_SOURCE_DIR}/libYARP_dev/FrameTransformClientTest.cpp)
       list(REMOVE_ITEM harness_code ${CMAKE_CURRENT_SOURCE_DIR}/libYARP_dev/MapGrid2DTest.cpp)
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
    target_link_libraries(${EXE} YARP::YARP_serversql)
    if(YARP_HAS_ACE)
      target_link_libraries(${EXE} ${ACE_LIBRARIES})
    endif()
    set_property(TARGET ${EXE} PROPERTY FOLDER "Test")

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

  # Compile AnalogWrapperTest only if fakeAnalog is enabled
  if(ENABLE_yarpmod_fakeAnalogSensor)
    add_definitions(-DYARP_ANALOGWRAPPER_TESTS)
  endif()

  # Compile TestFrameGrabberTest only if test_grabber is enabled
  if(ENABLE_yarpmod_test_grabber)
    add_definitions(-DYARP_TESTFRAMEGRABBER_TESTS)
  endif()

  add_subdirectory(carriers)
  add_subdirectory(devices)
  add_subdirectory(yarpidl_thrift)
  add_subdirectory(yarpidl_rosmsg)


  # Integration tests
  option(YARP_TEST_INTEGRATION "Run integration tests (UNIX only)" FALSE)
  mark_as_advanced(YARP_TEST_INTEGRATION)
  if(YARP_TEST_INTEGRATION)
    add_subdirectory(integration)
  endif()



  yarp_deprecated_option(TEST_yarpidl_rosmsg) # Since YARP 2.3.70
  yarp_deprecated_option(TEST_yarpidl_thrift) # Since YARP 2.3.70
  yarp_deprecated_option(CREATE_PLUGIN_TESTS) # Since YARP 2.3.70
  yarp_deprecated_option(ENABLE_YARPRUNTESTS) # Since YARP 2.3.70
endif()
