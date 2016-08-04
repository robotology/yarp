# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(CREATE_YARPMANAGER)

  include(YarpUseQt5)
  include(YarpOSXUtilities)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_manager_INCLUDE_DIRS TARGET YARP_manager PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_manager_INCLUDE_DIRS})

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  set(yarpmanager_SRCS main.cpp
                       yscopewindow.cpp
                       entitiestreewidget.cpp
                       stdoutwindow.cpp
                       moduleviewwidget.cpp
                       genericviewwidget.cpp
                       safe_manager.cpp
                       customtreewidget.cpp
                       resourceviewwidget.cpp
                       applicationviewwidget.cpp
                       logwidget.cpp
                       mainwindow.cpp
                       aboutdlg.cpp
                       genericinfodlg.cpp)
  set(yarpmanager_HDRS safe_manager.h
                       mainwindow.h
                       entitiestreewidget.h
                       template_res.h
                       applicationviewwidget.h
                       logwidget.h
                       resourceviewwidget.h
                       customtreewidget.h
                       moduleviewwidget.h
                       genericviewwidget.h
                       yscopewindow.h
                       stdoutwindow.h
                       aboutdlg.h
                          genericinfodlg.h)
  set(yarpmanager_QRC_FILES res.qrc)
  if(WIN32)
    set(yarpmanager_RC_FILES yarpmanager.rc)
  endif()
  set(yarpmanager_UI_FILES moduleviewwidget.ui
                           stdoutwindow.ui
                           yscopewindow.ui
                           mainwindow.ui
                           resourceviewwidget.ui
                           applicationviewwidget.ui
                           aboutdlg.ui
                           genericinfodlg.ui)

  qt5_add_resources(yarpmanager_QRC_GEN_SRCS ${yarpmanager_QRC_FILES})
  qt5_wrap_ui(yarpmanager_UI_GEN_SRCS ${yarpmanager_UI_FILES})

  source_group("Source Files" FILES ${yarpmanager_SRCS})
  source_group("Header Files" FILES ${yarpmanager_HDRS})
  source_group("Resources Files" FILES ${yarpmanager_QRC_FILES}
                                       ${yarpmanager_RC_FILES})
  source_group("UI Files" FILES ${yarpmanager_UI_FILES})
  source_group("Generated Files" FILES ${yarpmanager_QRC_GEN_SRCS}
                                       ${yarpmanager_UI_GEN_SRCS})

  add_executable(yarpmanager WIN32 ${yarpmanager_SRCS}
                                   ${yarpmanager_HDRS}
                                   ${yarpmanager_QRC_FILES}
                                   ${yarpmanager_QRC_GEN_SRCS}
                                   ${yarpmanager_RC_FILES}
                                   ${yarpmanager_UI_GEN_SRCS}
                                   ${yarpmanager_UI_FILES})

  target_link_libraries(yarpmanager YARP_OS
                                    YARP_init
                                    YARP_manager)
  qtyarp_use_modules(yarpmanager Widgets
                                 Gui)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    target_link_libraries(yarpmanager Qt5::WinMain)
  endif()

  install(TARGETS yarpmanager COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarpmanager
                                    TARGET_DEST yarpmanager.app
                                    APP_ICON Resources/AppIcon.icns
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

endif()
