# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(CREATE_YARPC "Compile experimental stable C library" FALSE)
mark_as_advanced(CREATE_YARPC)

if (CREATE_YARPC)
  option(CREATE_YARPC_STANDALONE "Make yarpc be fully standalone" TRUE)

  project(yarpc)

  set(YARP_COPY_SRC)
  if (CREATE_YARPC_STANDALONE)
    # in some cases, want to be fully stand-alone to not depend on C++ ABI.
    get_property(os_src TARGET YARP_OS PROPERTY SOURCES)
    get_property(sig_src TARGET YARP_sig PROPERTY SOURCES)
    get_property(dev_src TARGET YARP_dev PROPERTY SOURCES)
    get_property(init_src TARGET YARP_init PROPERTY SOURCES)
    set(YARP_COPY_SRC ${os_src} ${sig_src} ${dev_src} ${init_src})
    include_directories(${ACE_INCLUDE_DIRS})
    if (EXISTS ${CMAKE_BINARY_DIR}/libsupc++)
      file(GLOB supcxx ${CMAKE_BINARY_DIR}/libsupc++/*.cc)
      set(YARP_COPY_SRC ${YARP_COPY_SRC} ${supcxx})
      include_directories(${CMAKE_BINARY_DIR}/libsupc++)
    endif ()
    foreach (src ${YARP_COPY_SRC})
      set_property(SOURCE ${src} APPEND PROPERTY COMPILE_FLAGS 
         "-fvisibility=hidden")
    endforeach ()
  endif (CREATE_YARPC_STANDALONE)

  get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})
  include_directories(${PROJECT_SOURCE_DIR})

  add_library(yarpc yarp.h yarp.cpp yarpnetwork.cpp yarpport.cpp yarpcontact.cpp yarpreader.cpp yarpwriter.cpp yarpportable.cpp yarpstring.cpp yarpsemaphore.cpp yarpthread.cpp yarptime.cpp yarpbottle.cpp yarpimpl.h ${YARP_COPY_SRC})
  target_link_libraries(yarpc ${ACE_LIBRARIES})
  if (NOT CREATE_YARPC_STANDALONE)
    target_link_libraries(yarpc YARP_init)
  else (NOT CREATE_YARPC_STANDALONE)
    if (LINUX)
      target_link_libraries(yarpc pthread m dl rt)
    endif ()
    if (MINGW)
      target_link_libraries(yarpc stdc++)
      target_link_libraries(yarpc m ws2_32 mswsock netapi32 winmm)
      set_target_properties(yarpc PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libyarpc.def")
    endif ()
  endif (NOT CREATE_YARPC_STANDALONE)

  add_executable(yarpc_test1 test1.cpp)
  target_link_libraries(yarpc_test1 yarpc)

  add_executable(yarpc_test2 test2.cpp)
  target_link_libraries(yarpc_test2 yarpc)

endif (CREATE_YARPC)
