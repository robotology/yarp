#  Yarp Application Builder
#  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

if(CREATE_YARPBUILDER AND YARP_HAS_GTK2)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_manager_INCLUDE_DIRS TARGET YARP_manager PROPERTY INCLUDE_DIRS)

  set(folder_source src/application_list.cpp
                    src/application_model.cpp
                    src/application_window.cpp
                    src/application_wizard.cpp
                    src/app_property_window.cpp
                    src/arb_property_window.cpp
                    src/arrow_model.cpp
                    src/con_property_window.cpp
                    src/ext_port_model.cpp
                    src/yarpbuilder.cpp
                    src/int_port_model.cpp
                    src/label_model.cpp
                    src/main_window.cpp
                    src/message_list.cpp
                    src/midpoint_model.cpp
                    src/module_model.cpp
                    src/module_preview_window.cpp
                    src/module_property_window.cpp
                    src/module_window.cpp
                    src/port_abitrator_model.cpp
                    src/port_model.cpp
                    src/resource_window.cpp
                    src/tooltip_model.cpp)

  set(folder_header include/application_list.h
                    include/application_model.h
                    include/application_window.h
                    include/application_wizard.h
                    include/app_property_window.h
                    include/arb_property_window.h
                    include/arrow_model.h
                    include/con_property_window.h
                    include/custom_cellrender.h
                    include/ext_port_model.h
                    include/icon_res.h
                    include/int_port_model.h
                    include/label_model.h
                    include/main_window.h
                    include/message_list.h
                    include/midpoint_model.h
                    include/module_model.h
                    include/module_preview_window.h
                    include/module_property_window.h
                    include/module_window.h
                    include/port_abitrator_model.h
                    include/port_model.h
                    include/resource_window.h
                    include/template_res.h
                    include/tooltip_model.h)

  source_group("Source Files" FILES ${folder_source})
  source_group("Header Files" FILES ${folder_header})


  if(UNIX)
    if (GtkMM_VERSION_MAJOR GREATER 2 OR GtkMM_VERSION_MAJOR EQUAL 2)
      if (GtkMM_VERSION_MINOR LESS 22)
        add_definitions(-DGTKMM_LESS_2_22)
      endif()
    endif()

    add_definitions(-Wno-trigraphs)
  endif(UNIX)

  add_definitions(${GTK2_DEFINITIONS})

  include_directories(${YARP_OS_INCLUDE_DIRS})
  include_directories(${YARP_manager_INCLUDE_DIRS})
  include_directories(SYSTEM ${ACE_INCLUDE_DIRS})
  include_directories(SYSTEM ${TinyXML_INCLUDE_DIRS})
  include_directories(SYSTEM ${GTK2_INCLUDE_DIRS})
  include_directories(SYSTEM ${GooCanvasMM_INCLUDE_DIRS})
  include_directories(include)

  # GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES

  ## FIXME 2.4 Move the check in conf/YarpSystemCheck.cmake later
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag("-Wno-mismatched-tags" CXX_HAS_WNO_MISMATCHED_TAGS)
  if(CXX_HAS_WNO_MISMATCHED_TAGS)
    set_source_files_properties(${folder_source} PROPERTIES COMPILE_FLAGS "-Wno-mismatched-tags")
  endif(CXX_HAS_WNO_MISMATCHED_TAGS)

  add_executable(yarpbuilder-gtk ${folder_source} ${folder_header})

  target_link_libraries(yarpbuilder-gtk YARP_init
                                        YARP_OS
                                        YARP_manager
                                        ${ACE_LIBRARIES}
                                        ${GTK2_GTK_LIBRARY}
                                        ${GTK2_GDK_LIBRARY}
                                        ${GTK2_GLIB_LIBRARY}
                                        ${GTK2_PANGOCAIRO_LIBRARY}
                                        ${GTK2_GTKMM_LIBRARY}
                                        ${GTK2_GLIBMM_LIBRARY}
                                        ${GTK2_GDKMM_LIBRARY}
                                        ${GTK2_ATKMM_LIBRARY}
                                        ${GTK2_SIGC++_LIBRARY}
                                        ${GTK2_GTHREAD_LIBRARY}
                                        ${GooCanvasMM_LIBRARIES})

  install(TARGETS yarpbuilder-gtk
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  if(NOT TARGET yarpbuilder-qt OR YARP_DEFAULT_GTK)
    set_target_properties(yarpbuilder-gtk PROPERTIES OUTPUT_NAME yarpbuilder)
  endif()

endif()
