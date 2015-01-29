# Copyright (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(bayer_carrier TYPE BayerCarrier INCLUDE BayerCarrier.h)
  yarp_add_carrier_fingerprint(bayer.ini bayer_carrier)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_bayer_carrier)

  set(DC1394_FOUND FALSE)
  set(DC1394_SRC ${CMAKE_SOURCE_DIR}/extern/libdc1394_bayer/libdc1394_bayer.c)
  if (NOT WIN32)
    find_library(lib_raw1394 raw1394)
    find_library(lib_dc1394 dc1394)
    if (lib_raw1394 AND lib_dc1394)
      set(DC1394_FOUND TRUE)
      set(DC1394_LIBRARIES ${lib_raw1394} ${lib_dc1394})
    endif ()
  endif ()

  option(USE_LIBDC1394 "Link DC1394 externally" ${DC1394_FOUND})

  if (USE_LIBDC1394)
    add_definitions(-DUSE_LIBDC1394)
    set(DC1394_SRC)
  else ()
    include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/extern/libdc1394_bayer/)
  endif ()

  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  yarp_add_plugin(yarp_bayer BayerCarrier.h BayerCarrier.cpp ${DC1394_SRC})
  if (MSVC AND DC1394_SRC)
    set_source_files_properties(${DC1394_SRC} PROPERTIES LANGUAGE CXX)
    set_source_files_properties(${DC1394_SRC} PROPERTIES COMPILE_FLAGS "/wd4309 /wd4244 /wd4018")
  endif ()
  if (UNIX AND DC1394_SRC)
    set_source_files_properties(${DC1394_SRC} PROPERTIES COMPILE_FLAGS -Wno-sign-compare)
  endif ()
  target_link_libraries(yarp_bayer YARP_OS YARP_sig)
  target_link_libraries(yarp_bayer ${ACE_LIBRARIES})
  if (USE_LIBDC1394)
    target_link_libraries(yarp_bayer ${DC1394_LIBRARIES})
  endif()
endif ()
