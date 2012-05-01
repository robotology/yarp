
macro(yarp_idl thrift dir ARGN)
  get_filename_component(path_trift ${thrift} ABSOLUTE)
  if (NOT path_thrift) 
    set(path_thrift ${CMAKE_CURRENT_SOURCE_DIR})
  endif ()
  get_filename_component(name_thrift ${thrift} NAME)
  set(abs_thrift "${path_thrift}/${name_thrift}")
  message(STATUS "${thrift} : ${abs_thrift}")
  add_custom_command(OUTPUT ${ARGN}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${dir}
    COMMAND yarpidl_thrift -out ${dir} --gen yarp ${abs_thrift}
    DEPENDS ${thrift})  
endmacro()
