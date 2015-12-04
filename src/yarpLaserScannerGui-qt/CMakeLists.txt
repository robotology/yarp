# Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
# Author: Marco Randazzo marco.randazzo@iit.it
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

if(CREATE_YARPLASERSCANNERGUI AND YARP_HAS_QT5 AND YARP_HAS_OPENCV)

  include(YarpUseQt5)
  include(YarpOSXUtilities)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)

  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS}
					  ${OpenCV_INCLUDE_DIRS})

  set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

 # set(yarplaserscannergui_qt_SRCS main.cpp gui.cpp )
 # set(yarplaserscannergui_qt_HDRS gui.h )
 # set(yarplaserscannergui_qt_QRC_FILES res.qrc)
 # set(yarplaserscannergui_qt_UI_FILES gui.ui)

  set(yarplaserscannergui_qt_SRCS main.cpp)
 
 # qt5_add_resources(yarplaserscannergui_qt_QRC_GEN_SRCS $yarplaserscannergui_qt_QRC_FILES})
  qt5_wrap_ui(yarplaserscannergui_qt_UI_GEN_SRCS ${yarplaserscannerguii_qt_UI_FILES})

  source_group("Source Files" FILES ${yarplaserscannergui_qt_SRCS})
  source_group("Header Files" FILES ${yarplaserscannergui_qt_HDRS})
  source_group("Resources Files" FILES ${yarplaserscannergui_qt_QRC_FILES})
  source_group("UI Files" FILES ${yarplaserscannergui_qt_UI_FILES})
  source_group("Generated Files" FILES ${yarpLaserScannergui_qt_QRC_GEN_SRCS}
                                       ${yarplaserscannergui_qt_UI_GEN_SRCS})

 add_executable(yarplaserscannergui-qt WIN32  ${yarplaserscannergui_qt_SRCS}
                                        ${yarplaserscannergui_qt_HDRS}
                                        ${yarplaserscannergui_qt_QRC_GEN_SRCS}
                                        ${yarplaserscannergui_qt_UI_GEN_SRCS})

  target_link_libraries(yarplaserscannergui-qt YARP_OS
                                        YARP_init
                                        YARP_sig
                                        YARP_dev)
  qtyarp_use_modules(yarplaserscannergui-qt Widgets
                                     Gui)
  qtyarp_deprecate_with_cmake_version(2.8.11)
  if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    #target_link_libraries(yarplaserscannergui-qt Qt5::WinMain))
  endif()

   target_link_libraries(yarplaserscannergui-qt ${OpenCV_LIBRARIES})
	
  install(TARGETS yarplaserscannergui-qt COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  if(NOT YARP_DEFAULT_GTK)
    set_target_properties(yarplaserscannergui-qt PROPERTIES OUTPUT_NAME yarplaserscannergui)
  endif()

  yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarplaserscannergui-qt
                                    TARGET_DEST yarplaserscannergui-qt.app
                                    INSTALL_COMPONENT utilities
                                    INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

endif()
