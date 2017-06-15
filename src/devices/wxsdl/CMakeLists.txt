# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(wxsdl
                    CATEGORY device
                    TYPE yarp::dev::WxsdlWriter
                    INCLUDE WxsdlWriter.h
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_WXWIDGETS;YARP_HAS_SDL")

if(NOT SKIP_wxsdl)
  include_directories(SYSTEM ${wxWidgets_INCLUDE_DIRS}
                             ${SDL_INCLUDE_DIR})

  yarp_add_plugin(yarp_wxsdl WxsdlWriter.cpp
                             WxsdlWriter.h)
  target_link_libraries(yarp_wxsdl YARP::YARP_OS
                                   YARP::YARP_sig
                                   YARP::YARP_dev
                                   ${wxWidgets_LIBRARIES}
                                   ${SDL_LIBRARY})
  if(WIN32)
     target_link_libraries(winmm)
  endif()
  set_target_properties(yarp_wxsdl PROPERTIES COMPILE_DEFINITIONS "${wxWidgets_DEFINITIONS}")
  yarp_install(TARGETS yarp_wxsdl
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES wxsdl.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET vxsdl PROPERTY FOLDER "Unsupported")

  if(YARP_COMPILE_TESTS)
    add_executable(wxsdl_test WxsdlWriter.cpp
                              WxsdlWriter.h)
    target_link_libraries(wxsdl_test YARP::YARP_OS
                                     YARP::YARP_init
                                     YARP::YARP_sig
                                     YARP::YARP_dev
                                     ${wxWidgets_LIBRARIES}
                                     ${SDL_LIBRARY})
    set_target_properties(wxsdl_test PROPERTIES COMPILE_DEFINITIONS "${wxWidgets_DEFINITIONS} -DWXSDL_MAIN")
  endif()

endif()
