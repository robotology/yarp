# Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(CREATE_YARPMOTORGUI AND YARP_HAS_GTK2)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)

  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})
  include_directories(${GTK2_INCLUDE_DIRS})
  add_definitions(${GTK2_DEFINITIONS})

  set(yarpmotorgui_gtk_SRCS src/allPartsWindow.cpp
                            src/gtkMessages.cpp
                            src/guicontrol.cpp
                            src/guiPid2.cpp
                            src/main.cpp
                            src/multipleJointWindow.cpp
                            src/partMover.cpp
                            src/singleJointWindow.cpp
                            src/tableWindow.cpp
                            src/windowTools.cpp)

  set(yarpmotorgui_gtk_HDRS src/include/allPartsWindow.h
                            src/include/gtkMessages.h
                            src/include/guiControl.h
                            src/include/guiPid2.h
                            src/include/partMover.h
                            src/include/yarpmotorgui.h
                            src/include/windowTools.h)

  source_group("Source Files" FILES ${yarpmotorgui_gtk_SRCS})
  source_group("Header Files" FILES ${yarpmotorgui_gtk_HDRS})


  add_executable(yarpmotorgui-gtk ${yarpmotorgui_gtk_SRCS} ${yarpmotorgui_gtk_HDRS})

  target_link_libraries(yarpmotorgui-gtk YARP_OS
                                         YARP_init
                                         YARP_sig
                                         YARP_dev
                                         ${GTK2_LIBRARIES})

  if(UNIX)
    include(CheckCCompilerFlag)
    check_cxx_compiler_flag("-Wno-unused-variable" CXX_SUPPORTS_WNO_UNUSED_VARIABLE)
    if(CXX_SUPPORTS_WNO_UNUSED_VARIABLE)
      set_property(SOURCE ${yarpmotorgui_gtk_SRCS}
                   APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-unused-variable")
    endif()
    check_cxx_compiler_flag("-Wno-unused-but-set-variable" CXX_SUPPORTS_WNO_UNUSED_BUT_SET_VARIABLE)
    if(CXX_SUPPORTS_WNO_UNUSED_BUT_SET_VARIABLE)
      set_property(SOURCE ${yarpmotorgui_gtk_SRCS}
                   APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-unused-but-set-variable")
    endif()
    check_cxx_compiler_flag("-Wno-unused-function" CXX_SUPPORTS_WNO_UNUSED_FUNCTION)
    if(CXX_SUPPORTS_WNO_UNUSED_FUNCTION)
      set_property(SOURCE ${yarpmotorgui_gtk_SRCS}
                   APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-unused-function")
    endif()
    check_cxx_compiler_flag("-Wno-sign-compare" CXX_SUPPORTS_WNO_SIGN_COMPARE)
    if(CXX_SUPPORTS_WNO_SIGN_COMPARE)
      set_property(SOURCE src/singleJointWindow.cpp
                   APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-sign-compare")
    endif()
    check_cxx_compiler_flag("-Wno-missing-field-initializers" CXX_SUPPORTS_WNO_MISSING_FIELD_INITIALIZERS)
    if(CXX_SUPPORTS_WNO_MISSING_FIELD_INITIALIZERS)
      set_property(SOURCE src/tableWindow.cpp
                   APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-missing-field-initializers")
    endif()
  endif()

  install(TARGETS yarpmotorgui-gtk
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  if(NOT TARGET yarpmotorgui-qt OR YARP_DEFAULT_GTK)
    set_target_properties(yarpmotorgui-gtk PROPERTIES OUTPUT_NAME yarpmotorgui)
  endif()

  yarp_install(FILES yarpmotorgui.xml
               DESTINATION ${YARP_MODULES_INSTALL_DIR})

endif()
