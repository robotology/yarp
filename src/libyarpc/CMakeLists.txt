# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

option(CREATE_YARPC "Compile experimental stable C library" FALSE)
mark_as_advanced(CREATE_YARPC)

if (CREATE_YARPC)
  option(CREATE_YARPC_STANDALONE "Make yarpc be fully standalone" TRUE)

  project(yarpc)

  set(YARP_COPY_SRC)
  if (CREATE_YARPC_STANDALONE)
    # we had best be be fully stand-alone to not depend on C++ ABI.
    get_property(os_src TARGET YARP_OS PROPERTY SOURCES)
    get_property(sig_src TARGET YARP_sig PROPERTY SOURCES)
    get_property(dev_src TARGET YARP_dev PROPERTY SOURCES)
    get_property(init_src TARGET YARP_init PROPERTY SOURCES)
    set(YARP_COPY_SRC ${os_src} ${sig_src} ${dev_src} ${init_src})
    include_directories(${ACE_INCLUDE_DIRS})
  endif (CREATE_YARPC_STANDALONE)

  get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})
  include_directories(${PROJECT_SOURCE_DIR})

  add_library(yarpc yarp.h yarp.cpp yarpnetwork.cpp yarpport.cpp yarpcontact.cpp yarpreader.cpp yarpwriter.cpp yarpportable.cpp yarpstring.cpp yarpsemaphore.cpp yarpimpl.h ${YARP_COPY_SRC})
  if (NOT CREATE_YARPC_STANDALONE)
    target_link_libraries(yarpc libYARP_init)
  else (NOT CREATE_YARPC_STANDALONE)
    target_link_libraries(yarpc pthread)
  endif (NOT CREATE_YARPC_STANDALONE)

  add_executable(yarpc_test1 test1.cpp)
  target_link_libraries(yarpc_test1 yarpc)

  add_executable(yarpc_test2 test2.cpp)
  target_link_libraries(yarpc_test2 yarpc)

endif (CREATE_YARPC)
