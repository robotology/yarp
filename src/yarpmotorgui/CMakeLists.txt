# Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(CREATE_YARPMOTORGUI)

  include(YarpUseQt5)
  include(YarpOSXUtilities)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  set(yarpmotorgui_SRCS main.cpp
                        flowlayout.cpp
                        jointitem.cpp
                        mainwindow.cpp
                        partitem.cpp
                        piddlg.cpp
                        sequencewindow.cpp
                        sliderOptions.cpp
                        sliderWithTarget.cpp
                        startdlg.cpp)
  set(yarpmotorgui_HDRS flowlayout.h
                        jointitem.h
                        log.h
                        mainwindow.h
                        partitem.h
                        piddlg.h
                        sliderOptions.h
                        sliderWithTarget.h
                        yarpmotorgui.h
                        sequencewindow.h
                        startdlg.h)
  set(yarpmotorgui_QRC_FILES res.qrc)
  set(yarpmotorgui_UI_FILES jointitem.ui
                            mainwindow.ui
                            piddlg.ui
                            sequencewindow.ui
                            startdlg.ui
                            sliderOptions.ui)

  qt5_add_resources(yarpmotorgui_QRC_GEN_SRCS ${yarpmotorgui_QRC_FILES})
  qt5_wrap_ui(yarpmotorgui_UI_GEN_SRCS ${yarpmotorgui_UI_FILES})

  source_group("Source Files" FILES ${yarpmotorgui_SRCS})
  source_group("Header Files" FILES ${yarpmotorgui_HDRS})
  source_group("Resources Files" FILES ${yarpmotorgui_QRC_FILES})
  source_group("UI Files" FILES ${yarpmotorgui_UI_FILES})
  source_group("Generated Files" FILES ${yarpmotorgui_QRC_GEN_SRCS}
                                       ${yarpmotorgui_UI_GEN_SRCS})

  add_executable(yarpmotorgui WIN32  ${yarpmotorgui_SRCS}
                                     ${yarpmotorgui_HDRS}
                                     ${yarpmotorgui_QRC_GEN_SRCS}
                                     ${yarpmotorgui_UI_GEN_SRCS})

  target_link_libraries(yarpmotorgui YARP_OS
                                        YARP_init
                                        YARP_sig
                                        YARP_dev)
  qtyarp_use_modules(yarpmotorgui Widgets
                                  Gui)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    target_link_libraries(yarpmotorgui Qt5::WinMain)
  endif()

  install(TARGETS yarpmotorgui COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarpmotorgui
                                    TARGET_DEST yarpmotorgui.app
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")
  yarp_install(FILES yarpmotorgui.xml
               DESTINATION ${YARP_MODULES_INSTALL_DIR})
endif()
