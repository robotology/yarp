if(CREATE_YARPLOGGER AND YARP_HAS_QT5)

  project(QtYARPLogger)

  include(YarpUseQt5)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS})

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

  file(GLOB QtYARPLogger_SRCS *.cpp)
  file(GLOB QtYARPLogger_HDRS *.h)
  file(GLOB QtYARPLogger_QRC_FILES *.qrc)
  file(GLOB QtYARPLogger_UI_FILES *.ui)

  qt5_add_resources(QtYARPLogger_QRC_GEN_SRCS ${QtYARPLogger_QRC_FILES})
  qt5_wrap_ui(QtYARPLogger_UI_GEN_SRCS ${QtYARPLogger_UI_FILES})

  source_group("Source Files" FILES ${QtYARPLogger_SRCS})
  source_group("Header Files" FILES ${QtYARPLogger_HDRS})
  source_group("Resources Files" FILES ${QtYARPLogger_QRC_FILES})
  source_group("UI Files" FILES ${QtYARPLogger_UI_FILES})
  source_group("Generated Files" FILES ${QtYARPLogger_QRC_GEN_SRCS} ${QtYARPLogger_UI_GEN_SRCS})

  add_executable(QtYARPLogger WIN32 ${QtYARPLogger_SRCS}
                                    ${QtYARPLogger_HDRS}
                                    ${QtYARPLogger_QRC_GEN_SRCS}
                                    ${QtYARPLogger_UI_GEN_SRCS})
  target_link_libraries(QtYARPLogger YARP_OS
                                     YARP_init)
  qt5_use_modules(QtYARPLogger Widgets
                               Gui)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32)
       target_link_libraries(QtYARPLogger Qt5::WinMain)
  endif()

  install(TARGETS QtYARPLogger
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()