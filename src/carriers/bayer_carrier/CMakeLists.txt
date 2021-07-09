# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(bayer
                    CATEGORY carrier
                    TYPE BayerCarrier
                    INCLUDE BayerCarrier.h
                    EXTRA_CONFIG SUBTYPE=recv
                                 CODE="not applicable"
                    DEFAULT ON)

if(NOT SKIP_bayer)
  yarp_add_plugin(yarp_bayer)

  if(NOT YARP_HAS_Libdc1394)
    set(DC1394_SRC ${libdc1394_bayer_ROOT}/libdc1394_bayer.c
                   ${libdc1394_bayer_ROOT}/conversions.h)
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

  target_sources(yarp_bayer PRIVATE BayerCarrier.h
                                    BayerCarrier.cpp
                                    ${DC1394_SRC})

  target_link_libraries(yarp_bayer PRIVATE YARP::YARP_os
                                           YARP::YARP_sig)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig)

  target_include_directories(yarp_bayer SYSTEM PRIVATE ${libdc1394_bayer_ROOT})
  target_link_libraries(yarp_bayer PRIVATE ${Libdc1394_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS Libdc1394) (not using targets)

  yarp_install(TARGETS yarp_bayer
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_bayer PROPERTY FOLDER "Plugins/Carrier")
endif()
