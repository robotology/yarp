# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarpmanager)

  include(YarpUseQt5)
  include(YarpMacOSUtilities)

  add_executable(yarpmanager WIN32)

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  # Manager files
  set(yarpmanager_manager_SRCS src-manager/aboutdlg.cpp
                               src-manager/applicationviewwidget.cpp
                               src-manager/clusterWidget.cpp
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
                               src-manager/clusterWidget.h
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
                                   src-manager/clusterWidget.ui
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

  target_sources(yarpmanager PRIVATE ${yarpmanager_SRCS}
                                     ${yarpmanager_HDRS}
                                     ${yarpmanager_QRC_FILES}
                                     ${yarpmanager_QRC_GEN_SRCS}
                                     ${yarpmanager_RC_FILES}
                                     ${yarpmanager_UI_GEN_SRCS}
                                     ${yarpmanager_UI_FILES})

  target_include_directories(yarpmanager PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src-builder"
                                                  "${CMAKE_CURRENT_LIST_DIR}/src-manager")
  if(MSVC)
    target_include_directories(yarpmanager SYSTEM PRIVATE ${dirent_INCLUDE_DIRS})
  endif()

  target_link_libraries(yarpmanager PRIVATE YARP::YARP_os
                                            YARP::YARP_init
                                            YARP::YARP_manager
                                            YARP::YARP_profiler
                                            Qt5::Widgets
                                            Qt5::Gui)

  install(TARGETS yarpmanager COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarpmanager PROPERTY FOLDER "Graphical User Interfaces")

  yarp_macos_duplicate_and_add_bundle(TARGET yarpmanager
                                      APP_ICON Resources/AppIcon.icns
                                      INSTALL_COMPONENT utilities
                                      INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

endif()
