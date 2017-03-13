# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#          Ali Paikan <ali.paikan@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(CREATE_YARPMANAGER)

  include(YarpUseQt5)
  include(YarpOSXUtilities)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_manager_INCLUDE_DIRS TARGET YARP_manager PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_manager_INCLUDE_DIRS}
                      ./src-builder
                      ./src-manager)

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  # Manager files
  set(yarpmanager_manager_SRCS src-manager/aboutdlg.cpp
                               src-manager/applicationviewwidget.cpp
                               src-manager/customtreewidget.cpp
                               src-manager/entitiestreewidget.cpp
                               src-manager/genericinfodlg.cpp
                               src-manager/genericviewwidget.cpp
                               src-manager/logwidget.cpp
                               src-manager/main.cpp
                               src-manager/mainwindow.cpp
                               src-manager/moduleviewwidget.cpp
                               src-manager/newapplicationwizard.cpp
                               src-manager/resourceviewwidget.cpp
                               src-manager/safe_manager.cpp
                               src-manager/stdoutwindow.cpp
                               src-manager/yscopewindow.cpp)

  set(yarpmanager_manager_HDRS src-manager/aboutdlg.h
                               src-manager/applicationviewwidget.h
                               src-manager/customtreewidget.h
                               src-manager/entitiestreewidget.h
                               src-manager/genericinfodlg.h
                               src-manager/genericviewwidget.h
                               src-manager/logwidget.h
                               src-manager/mainwindow.h
                               src-manager/moduleviewwidget.h
                               src-manager/newapplicationwizard.h
                               src-manager/resourceviewwidget.h
                               src-manager/safe_manager.h
                               src-manager/stdoutwindow.h
                               src-manager/template_res.h
                               src-manager/yscopewindow.h)

  set(yarpmanager_manager_UI_FILES src-manager/aboutdlg.ui
                                   src-manager/applicationviewwidget.ui
                                   src-manager/genericinfodlg.ui
                                   src-manager/mainwindow.ui
                                   src-manager/moduleviewwidget.ui
                                   src-manager/resourceviewwidget.ui
                                   src-manager/stdoutwindow.ui
                                   src-manager/yscopewindow.ui)


  # Builder files
  set(yarpmanager_builder_HDRS src-builder/applicationitem.h
                               src-builder/arrow.h
                               src-builder/builderitem.h
                               src-builder/builderscene.h
                               src-builder/builderwindow.h
                               src-builder/commons.h
                               src-builder/customtree.h
                               src-builder/destinationportitem.h
                               src-builder/itemsignalhandler.h
                               src-builder/moduleitem.h
                               src-builder/propertiestable.h
                               src-builder/sourceportitem.h
                               src-builder/yarpbuilderlib_global.h
                               src-builder/yarpbuilderlib.h)

  set(yarpmanager_builder_SRCS src-builder/applicationitem.cpp
                               src-builder/arrow.cpp
                               src-builder/builderitem.cpp
                               src-builder/builderscene.cpp
                               src-builder/builderwindow.cpp
                               src-builder/customtree.cpp
                               src-builder/destinationportitem.cpp
                               src-builder/itemsignalhandler.cpp
                               src-builder/moduleitem.cpp
                               src-builder/propertiestable.cpp
                               src-builder/sourceportitem.cpp
                               src-builder/yarpbuilderlib.cpp)


  # All files
  set(yarpmanager_HDRS ${yarpmanager_manager_HDRS}
                       ${yarpmanager_builder_HDRS})

  set(yarpmanager_SRCS ${yarpmanager_manager_SRCS}
                       ${yarpmanager_builder_SRCS})

  set(yarpmanager_UI_FILES ${yarpmanager_manager_UI_FILES})


  # Resource files
  set(yarpmanager_QRC_FILES res.qrc)
  if(WIN32)
    set(yarpmanager_RC_FILES yarpmanager.rc)
  endif()


  # Generated files
  qt5_add_resources(yarpmanager_QRC_GEN_SRCS ${yarpmanager_QRC_FILES})
  qt5_wrap_ui(yarpmanager_UI_GEN_SRCS ${yarpmanager_UI_FILES})

  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
               PREFIX "Source Files"
               FILES ${yarpmanager_manager_SRCS}
                     ${yarpmanager_builder_SRCS})
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
               PREFIX "Header Files"
               FILES ${yarpmanager_manager_HDRS}
                     ${yarpmanager_builder_HDRS})
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

  target_link_libraries(yarpmanager YARP::YARP_OS
                                    YARP::YARP_init
                                    YARP::YARP_manager
                                    Qt5::Widgets
                                    Qt5::Gui)

  install(TARGETS yarpmanager COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarpmanager PROPERTY FOLDER "Graphical User Interfaces")

  yarp_osx_duplicate_and_add_bundle(TARGET yarpmanager
                                    APP_ICON Resources/AppIcon.icns
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

endif()
