if(CREATE_YARPLOGGER)

  project(yarplogger)

  include(YarpUseQt5)
  include(YarpOSXUtilities)

  get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_logger_HDRS TARGET YARP_logger PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS} ${YARP_logger_HDRS})
  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  file(GLOB yarplogger_SRCS *.cpp)
  file(GLOB yarplogger_HDRS *.h)
  file(GLOB yarplogger_QRC_FILES *.qrc)
  file(GLOB yarplogger_UI_FILES *.ui)

  qt5_add_resources(yarplogger_QRC_GEN_SRCS ${yarplogger_QRC_FILES})
  qt5_wrap_ui(yarplogger_UI_GEN_SRCS ${yarplogger_UI_FILES})

  include_directories(${YARP_TREE_INCLUDE_DIRS})
  source_group("Source Files" FILES ${yarplogger_SRCS})
  source_group("Header Files" FILES ${yarplogger_HDRS} ${YARP_logger_HDRS})
  source_group("Resources Files" FILES ${yarplogger_QRC_FILES})
  source_group("UI Files" FILES ${yarplogger_UI_FILES})
  source_group("Generated Files" FILES ${yarplogger_QRC_GEN_SRCS} ${yarplogger_UI_GEN_SRCS})

  add_executable(yarplogger WIN32 ${yarplogger_SRCS}
                                  ${yarplogger_HDRS}
                                  ${yarplogger_QRC_GEN_SRCS}
                                  ${yarplogger_UI_GEN_SRCS})

  target_link_libraries(yarplogger YARP_OS
                                   YARP_init
                                   YARP_logger)
  qtyarp_use_modules(yarplogger Widgets
                                Gui)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32)
       target_link_libraries(yarplogger Qt5::WinMain)
  endif()

  install(TARGETS yarplogger
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarplogger
                                    TARGET_DEST yarplogger.app
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

endif()
