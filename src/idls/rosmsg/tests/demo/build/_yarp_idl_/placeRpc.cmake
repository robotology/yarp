# Copyright: (C) 2012 IITRBCS
# Authors: Elena Ceseracciu
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

FILE(GLOB globbedHeaders RELATIVE /opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/_yarp_idl_/ "/opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/_yarp_idl_/*.h")
FILE(GLOB globbedSources RELATIVE /opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/_yarp_idl_/ "/opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/_yarp_idl_/*.cpp")

set(headers)
set(sources)

set(prefix)
set(include_prefix )
if (include_prefix)
  set(prefix ${include_prefix}/)
endif()

# check that globbed files are really the ones we need from generated listfile
FILE(STRINGS /opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/_yarp_idl_/Rpc_indexALL.txt index)
#prepare include and src directories in the source tree
make_directory("/opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/bits/include/${prefix}")
make_directory("/opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/bits/src/${prefix}")


foreach(genFile ${index})
  get_filename_component(type ${genFile} EXT)
  get_filename_component(main ${genFile} NAME_WE)
  if (${type} STREQUAL ".h")
    list(FIND globbedHeaders ${genFile} inclFound)
    if (${inclFound} EQUAL -1)
      message(WARNING "${genFile} not found in Rpc dir!")
    else(${inclFound} EQUAL -1) 
      FILE(COPY /opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/_yarp_idl_/${genFile} DESTINATION /opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/bits/include/${prefix})
      list(APPEND headers "include/${prefix}${genFile}")
    endif(${inclFound} EQUAL -1)
    list(FIND globbedHeaders ${main}Reply${type} inclFound)
    if (NOT ${inclFound} EQUAL -1)
      FILE(COPY /opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/_yarp_idl_/${main}Reply${type} DESTINATION /opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/bits/include/${prefix})
      list(APPEND headers "include/${prefix}${main}Reply${type}")
    endif(NOT ${inclFound} EQUAL -1)
  elseif(${type} STREQUAL ".cpp")
    list(FIND globbedSources ${genFile} srcFound)
    if (${srcFound} EQUAL -1)
      message(WARNING "${genFile} not found in Rpc dir!")
    else(${srcFound} EQUAL -1) 
      FILE(COPY /opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/_yarp_idl_/${genFile} DESTINATION /opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/bits/src/${prefix})
      list(APPEND sources "src/${prefix}${genFile}")
    endif(${srcFound} EQUAL -1)

  else(${type} STREQUAL ".h")
message(WARNING "Filename extension of ${genFile} is neither .h nor .cpp")
  endif(${type} STREQUAL ".h")
endforeach(genFile)

file(WRITE /opt/iit/src/yarp/src/idls/rosmsg/tests/demo/build/bits/Rpc_srv.cmake "## This is an automatically-generated file.\n## It could get re-generated if the ALLOW_IDL_GENERATION flag is on\n\nset(headers ${headers})\nset(sources ${sources})\n")
