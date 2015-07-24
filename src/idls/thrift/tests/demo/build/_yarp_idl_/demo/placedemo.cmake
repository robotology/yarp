# Copyright: (C) 2012 IITRBCS
# Authors: Elena Ceseracciu
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

file(GLOB globbedHeaders RELATIVE /opt/iit/src/yarp/src/idls/thrift/tests/demo/build/_yarp_idl_/demo/ "/opt/iit/src/yarp/src/idls/thrift/tests/demo/build/_yarp_idl_/demo/*.h")
file(GLOB globbedSources RELATIVE /opt/iit/src/yarp/src/idls/thrift/tests/demo/build/_yarp_idl_/demo/ "/opt/iit/src/yarp/src/idls/thrift/tests/demo/build/_yarp_idl_/demo/*.cpp")

set(headers)
set(sources)

set(prefix)
set(include_prefix )
if (include_prefix)
  set(prefix ${include_prefix}/)
endif()

# check that globbed files are really the ones we need from generated listfile
file(STRINGS /opt/iit/src/yarp/src/idls/thrift/tests/demo/build/_yarp_idl_/demo/demo_indexALL.txt index)
#prepare include and src directories in the source tree
make_directory("/opt/iit/src/yarp/src/idls/thrift/tests/demo/build/include/${prefix}")
make_directory("/opt/iit/src/yarp/src/idls/thrift/tests/demo/build/src/${prefix}")

foreach(genFile ${index})
  get_filename_component(type ${genFile} EXT)
  get_filename_component(main ${genFile} NAME_WE)
  if(${type} STREQUAL ".h")
    list(FIND globbedHeaders ${genFile} inclFound)
    if(${inclFound} EQUAL -1)
      message(WARNING "${genFile} not found in demo dir!")
    else()
      file(COPY /opt/iit/src/yarp/src/idls/thrift/tests/demo/build/_yarp_idl_/demo/${genFile} DESTINATION /opt/iit/src/yarp/src/idls/thrift/tests/demo/build/include/${prefix})
      list(APPEND headers "include/${prefix}${genFile}")
    endif()
    list(FIND globbedHeaders ${main}Reply${type} inclFound)
    if (NOT ${inclFound} EQUAL -1)
      file(COPY /opt/iit/src/yarp/src/idls/thrift/tests/demo/build/_yarp_idl_/demo/${main}Reply${type} DESTINATION /opt/iit/src/yarp/src/idls/thrift/tests/demo/build/include/${prefix})
      list(APPEND headers "include/${prefix}${main}Reply${type}")
    endif()
  elseif(${type} STREQUAL ".cpp")
    list(FIND globbedSources ${genFile} srcFound)
    if (${srcFound} EQUAL -1)
      message(WARNING "${genFile} not found in demo dir!")
    else()
      file(COPY /opt/iit/src/yarp/src/idls/thrift/tests/demo/build/_yarp_idl_/demo/${genFile} DESTINATION /opt/iit/src/yarp/src/idls/thrift/tests/demo/build/src/${prefix})
      list(APPEND sources "src/${prefix}${genFile}")
    endif()

  else()
    message(WARNING "Filename extension of ${genFile} is neither .h nor .cpp")
  endif()
endforeach(genFile)

file(WRITE /opt/iit/src/yarp/src/idls/thrift/tests/demo/build/demo_thrift.cmake
"## This is an automatically-generated file.
## It could get re-generated if the ALLOW_IDL_GENERATION flag is on

set(headers ${headers})
set(sources ${sources})")
