# Copyright: (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set(tests ${CMAKE_CURRENT_SOURCE_DIR})
set(outs ${CMAKE_CURRENT_BINARY_DIR})

macro(add_gen_test name)
  add_test(NAME ${name} COMMAND $<TARGET_FILE:yarpidl_thrift> -out ${outs} --gen yarp ${ARGN})
endmacro()

macro(add_config_test name dir)
  set(bin_dir ${CMAKE_CURRENT_BINARY_DIR}/make/${dir})
  file(MAKE_DIRECTORY ${bin_dir})
  add_test(NAME ${name} WORKING_DIRECTORY ${bin_dir}
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DALLOW_IDL_GENERATION=TRUE -DYARP_DIR=${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${dir} )
endmacro()

macro(add_make_test name dir)
  set(bin_dir ${CMAKE_CURRENT_BINARY_DIR}/make/${dir})
  add_test(NAME ${name} COMMAND ${CMAKE_COMMAND} --build ${bin_dir})
endmacro()

macro(add_test_test name dir)
  set(bin_dir ${CMAKE_CURRENT_BINARY_DIR}/make/${dir})
  add_test(NAME ${name} 
    WORKING_DIRECTORY ${bin_dir}
    COMMAND ${CMAKE_CTEST_COMMAND})
endmacro()


add_gen_test(idl_thrift_demo_base ${tests}/demo/demo.thrift)
# set_property(TEST idl_demo PROPERTIES DEPENDS idl_demo_base)

add_config_test(idl_thrift_demo_config demo) 

add_make_test(idl_thrift_demo_make demo) 

add_test_test(idl_thrift_demo_test demo) 
