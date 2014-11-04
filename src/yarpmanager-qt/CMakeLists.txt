if(CREATE_GYARPMANAGER AND YARP_HAS_QT5 AND CREATE_YMANAGER)

  include(YarpUseQt5)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_manager_INCLUDE_DIRS TARGET YARP_manager PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_manager_INCLUDE_DIRS})

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  set(yarpmanager_qt_SRCS main.cpp
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
                          mainwindow.cpp)
  set(yarpmanager_qt_HDRS safe_manager.h
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
                          stdoutwindow.h)
  set(yarpmanager_qt_QRC_FILES res.qrc)
  set(yarpmanager_qt_UI_FILES moduleviewwidget.ui
                              stdoutwindow.ui
                              yscopewindow.ui
                              mainwindow.ui
                              resourceviewwidget.ui
                              applicationviewwidget.ui)

  qt5_add_resources(yarpmanager_qt_QRC_GEN_SRCS ${yarpmanager_qt_QRC_FILES})
  qt5_wrap_ui(yarpmanager_qt_UI_GEN_SRCS ${yarpmanager_qt_UI_FILES})

  source_group("Source Files" FILES ${yarpmanager_qt_SRCS})
  source_group("Header Files" FILES ${yarpmanager_qt_HDRS})
  source_group("Resources Files" FILES ${yarpmanager_qt_QRC_FILES})
  source_group("UI Files" FILES ${yarpmanager_qt_UI_FILES})
  source_group("Generated Files" FILES ${yarpmanager_qt_QRC_GEN_SRCS}
                                      ${yarpmanager_qt_UI_GEN_SRCS})

  add_executable(yarpmanager-qt WIN32 ${yarpmanager_qt_SRCS}
                                      ${yarpmanager_qt_HDRS}
                                      ${yarpmanager_qt_QRC_GEN_SRCS}
                                      ${yarpmanager_qt_UI_GEN_SRCS})
  target_link_libraries(yarpmanager-qt YARP_OS
                                       YARP_init
                                       YARP_manager)
  qt5_use_modules(yarpmanager-qt Widgets
                                Gui)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    target_link_libraries(yarpmanager-qt Qt5::WinMain)
  endif()

  install(TARGETS yarpmanager-qt COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  if(NOT YARP_DEFAULT_GTK)
    set_target_properties(yarpmanager-qt PROPERTIES OUTPUT_NAME yarpmanager)
  endif()

  # Keep compatibility with the old name
  if(NOT YARP_NO_DEPRECATED AND NOT YARP_DEFAULT_GTK)
    add_executable(gyarpmanager WIN32 ${yarpmanager_qt_SRCS}
                                      ${yarpmanager_qt_HDRS}
                                      ${yarpmanager_qt_QRC_GEN_SRCS}
                                      ${yarpmanager_qt_UI_GEN_SRCS})
    target_link_libraries(gyarpmanager YARP_OS
                                       YARP_init
                                       YARP_manager)
    qt5_use_modules(gyarpmanager Widgets
                                 Gui)
    qtyarp_deprecate_with_cmake_version(2.8.11)
    if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
      target_link_libraries(gyarpmanager Qt5::WinMain)
    endif()

    install(TARGETS gyarpmanager COMPONENT deprecated DESTINATION ${CMAKE_INSTALL_BINDIR})
  endif()

endif()
