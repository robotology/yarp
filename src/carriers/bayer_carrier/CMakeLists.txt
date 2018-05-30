# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(bayer
                    CATEGORY carrier
                    TYPE yarp::os::BayerCarrier
                    INCLUDE BayerCarrier.h
                    EXTRA_CONFIG SUBTYPE=recv
                                 CODE="not applicable"
                    DEFAULT ON)

if(NOT SKIP_bayer)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  if(NOT YARP_HAS_LIBDC1394)
    set(DC1394_SRC ${libdc1394_bayer_ROOT}/libdc1394_bayer.c
                   ${libdc1394_bayer_ROOT}/conversions.h)
    include_directories(SYSTEM ${libdc1394_bayer_ROOT})
    set(Libdc1394_LIBRARIES)
    if (MSVC AND DC1394_SRC)
      if(MSVC_VERSION VERSION_LESS 1900)
        # Visual studio 2015 no longer needs this
        # When this if is no longer needed, remember to fix also
        # extern/libdc1394_bayer/conversions.h
        set_source_files_properties(${DC1394_SRC} PROPERTIES LANGUAGE CXX)
      endif()
      set_source_files_properties(${DC1394_SRC} PROPERTIES COMPILE_FLAGS "/wd4309 /wd4244 /wd4018")
    endif()
    if (UNIX AND DC1394_SRC)
      set_source_files_properties(${DC1394_SRC} PROPERTIES COMPILE_FLAGS -Wno-sign-compare)
    endif()
  else()
    set(DC1394_SRC)
    add_definitions(-DUSE_LIBDC1394)
  endif()

  yarp_add_plugin(yarp_bayer BayerCarrier.h
                             BayerCarrier.cpp
                             ${DC1394_SRC})

  target_link_libraries(yarp_bayer YARP::YARP_OS
                                   YARP::YARP_sig
                                   ${Libdc1394_LIBRARIES})

  yarp_install(TARGETS yarp_bayer
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES bayer.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_bayer PROPERTY FOLDER "Plugins/Carrier")
endif()
