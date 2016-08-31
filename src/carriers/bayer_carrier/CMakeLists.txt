# Copyright (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(bayer
                    CATEGORY carrier
                    TYPE yarp::os::BayerCarrier
                    INCLUDE BayerCarrier.h
                    EXTRA_CONFIG SUBTYPE=recv
                                 CODE="not applicable"
                    DEPENDS CREATE_OPTIONAL_CARRIERS
                    DEFAULT ON
                    ADVANCED ON)

if(NOT SKIP_bayer)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS})

  if(NOT YARP_HAS_LIBDC1394)
    set(DC1394_SRC ${CMAKE_SOURCE_DIR}/extern/libdc1394_bayer/libdc1394_bayer.c)
    include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/extern/libdc1394_bayer/)
    set(Libdc1394_LIBRARIES )
    if (MSVC AND DC1394_SRC)
      set_source_files_properties(${DC1394_SRC} PROPERTIES LANGUAGE CXX)
      set_source_files_properties(${DC1394_SRC} PROPERTIES COMPILE_FLAGS "/wd4309 /wd4244 /wd4018")
    endif ()
    if (UNIX AND DC1394_SRC)
      set_source_files_properties(${DC1394_SRC} PROPERTIES COMPILE_FLAGS -Wno-sign-compare)
    endif ()
  else()
    set(DC1394_SRC )
    add_definitions(-DUSE_LIBDC1394)
  endif()

  yarp_add_plugin(yarp_bayer BayerCarrier.h
                             BayerCarrier.cpp
                             ${DC1394_SRC})

  target_link_libraries(yarp_bayer YARP_OS
                                   YARP_sig
                                   ${Libdc1394_LIBRARIES})

  yarp_install(TARGETS yarp_bayer
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES bayer.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()
