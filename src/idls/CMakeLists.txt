option(CREATE_IDLS "Compile (very experimental) IDL compilers" FALSE)

if (CREATE_IDLS)
   option(ENABLE_yarpidl_thrift "Use a thrift-based IDL" OFF)
   if (ENABLE_yarpidl_thrift)
     add_subdirectory(thrift)
   endif ()
endif ()