# Copyright: (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

get_target_property(yarpidl_rosmsg yarpidl_rosmsg LOCATION)
set(tests ${CMAKE_CURRENT_SOURCE_DIR})
set(outs ${CMAKE_CURRENT_BINARY_DIR})

macro(add_gen_test name)
  add_test(${name} ${yarpidl_rosmsg} ${ARGN} --out ${outs} --gen yarp)
endmacro()

macro(add_config_test name dir)
  set(bin_dir ${CMAKE_CURRENT_BINARY_DIR}/make/${dir})
  file(MAKE_DIRECTORY ${bin_dir})
  add_test(NAME ${name} WORKING_DIRECTORY ${bin_dir}
    COMMAND ${CMAKE_COMMAND} -DALLOW_IDL_GENERATION=TRUE ${CMAKE_CURRENT_SOURCE_DIR}/${dir} )
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


add_gen_test(idl_rosmsg_demo_base ${tests}/demo/Demo.msg)

add_config_test(idl_rosmsg_demo_config demo) 

add_make_test(idl_rosmsg_demo_make demo) 

add_test_test(idl_rosmsg_demo_test demo) 
