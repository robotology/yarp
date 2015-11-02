if(CREATE_YARPLOGGER AND YARP_HAS_QT5)

  project(yarplogger-qt)

  include(YarpUseQt5)
  include(YarpOSXUtilities)

  get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_logger_HDRS TARGET YARP_logger PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS} ${YARP_logger_HDRS})
  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  file(GLOB yarplogger-qt_SRCS *.cpp)
  file(GLOB yarplogger-qt_HDRS *.h)
  file(GLOB yarplogger-qt_QRC_FILES *.qrc)
  file(GLOB yarplogger-qt_UI_FILES *.ui)

  qt5_add_resources(yarplogger-qt_QRC_GEN_SRCS ${yarplogger-qt_QRC_FILES})
  qt5_wrap_ui(yarplogger-qt_UI_GEN_SRCS ${yarplogger-qt_UI_FILES})

  include_directories(${YARP_TREE_INCLUDE_DIRS})
  source_group("Source Files" FILES ${yarplogger-qt_SRCS})
  source_group("Header Files" FILES ${yarplogger-qt_HDRS} ${YARP_logger_HDRS})
  source_group("Resources Files" FILES ${yarplogger-qt_QRC_FILES})
  source_group("UI Files" FILES ${yarplogger-qt_UI_FILES})
  source_group("Generated Files" FILES ${yarplogger-qt_QRC_GEN_SRCS} ${yarplogger-qt_UI_GEN_SRCS})

  add_executable(yarplogger-qt WIN32 ${yarplogger-qt_SRCS}
                                     ${yarplogger-qt_HDRS}
                                     ${yarplogger-qt_QRC_GEN_SRCS}
                                     ${yarplogger-qt_UI_GEN_SRCS})

  target_link_libraries(yarplogger-qt YARP_OS
                                      YARP_init
                                      YARP_logger)
  qtyarp_use_modules(yarplogger-qt Widgets
                                   Gui)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32)
       target_link_libraries(yarplogger-qt Qt5::WinMain)
  endif()

  install(TARGETS yarplogger-qt
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_target_properties(yarplogger-qt PROPERTIES OUTPUT_NAME yarplogger)

  yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarplogger-qt
                                    TARGET_DEST yarplogger-qt.app
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

endif()
