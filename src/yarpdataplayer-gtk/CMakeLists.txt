# Copyright: (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
# Authors: Vadim Tikhanoff
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

if(CREATE_YARPDATAPLAYER AND YARP_HAS_GTK2)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)

  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                      ${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${GTK2_INCLUDE_DIRS})

  if(YARP_HAS_OPENCV)
    add_definitions(-DHAS_OPENCV)
    include_directories(${OpenCV_INCLUDE_DIRS})
  else()
    message(STATUS "yarpdataplayer-gtk: OpenCV not selected, keep on building...")
  endif()

  set(yarpdataplayer_gtk_SRCS src/main.cpp
                              src/main_window.cpp
                              src/utils.cpp
                              src/worker.cpp)
  set(yarpdataplayer_gtk_HDRS include/main_window.h
                              include/utils.h
                              include/worker.h)
  if(MSVC)
    list(APPEND yarpdataplayer_gtk_HDRS include/msvc/dirent.h)
  endif()
  set(yarpdataplayer_gtk_THRIFT_FILES dataSetPlayer.thrift)

  include(YarpIDL)
  yarp_idl_to_dir(${yarpdataplayer_gtk_THRIFT_FILES}
                  ${CMAKE_CURRENT_SOURCE_DIR}/idl_generated_code
                  yarpdataplayer_gtk_THRIFT_GEN_SRCS
                  yarpdataplayer_gtk_THRIFT_GEN_HDRS
                  yarpdataplayer_gtk_THRIFT_INCLUDE_DIRS)
  include_directories(${yarpdataplayer_gtk_THRIFT_INCLUDE_DIRS})

  source_group("Source Files" FILES ${yarpdataplayer_gtk_SRCS})
  source_group("Header Files" FILES ${yarpdataplayer_gtk_HDRS})
  source_group("Thrift Files" FILES ${yarpdataplayer_gtk_THRIFT_FILES})
  source_group("Generated Files" FILES ${yarpdataplayer_gtk_THRIFT_GEN_SRCS}
                                       ${yarpdataplayer_gtk_THRIFT_GEN_HDRS})

  add_executable(yarpdataplayer-gtk ${yarpdataplayer_gtk_SRCS}
                                    ${yarpdataplayer_gtk_HDRS}
                                    ${yarpdataplayer_gtk_THRIFT_GEN_SRCS}
                                    ${yarpdataplayer_gtk_THRIFT_GEN_HDRS})

  target_link_libraries(yarpdataplayer-gtk YARP_OS
                                           YARP_init
                                           YARP_sig
                                           ${GTK2_LIBRARIES})

  if(YARP_HAS_OPENCV)
    target_link_libraries(yarpdataplayer-qt ${OpenCV_LIBRARIES})
  endif()

  install(TARGETS yarpdataplayer-gtk
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  if(NOT TARGET yarpdataplayer-qt OR YARP_DEFAULT_GTK)
    set_target_properties(yarpdataplayer-gtk PROPERTIES OUTPUT_NAME yarpdataplayer)
  endif()

endif()
