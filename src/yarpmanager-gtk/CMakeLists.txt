#  Yarp Modules Manager
#  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

if(CREATE_YARPMANAGER AND YARP_HAS_GTK2)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_manager_INCLUDE_DIRS TARGET YARP_manager PROPERTY INCLUDE_DIRS)

  include_directories(${GTK2_INCLUDE_DIRS})
  include_directories(${YARP_OS_INCLUDE_DIRS})
  include_directories(${YARP_manager_INCLUDE_DIRS})
  include_directories(${ACE_INCLUDE_DIRS})
  include_directories(${TinyXML_INCLUDE_DIRS})
  add_definitions(${GTK2_DEFINITIONS})

  set(yarpmanager_gtk_SRCS application_list.cpp
                           application_window.cpp
                           yarpmanager.cpp
                           main_window.cpp
                           message_list.cpp
                           module_window.cpp
                           resource_window.cpp
                           safe_manager.cpp
                           stdout_window.cpp)

  set(yarpmanager_gtk_HDRS application_list.h
                           application_window.h
                           icon_res.h
                           main_window.h
                           message_list.h
                           module_window.h
                           resource_window.h
                           safe_manager.h
                           stdout_window.h
                           template_res.h
                           yscope_window.h)

  source_group("Source Files" FILES ${yarpmanager_gtk_SRCS})
  source_group("Header Files" FILES ${yarpmanager_gtk_HDRS})


  if(MSVC)
    add_definitions(/wd4099)
  endif(MSVC)

  ## FIXME 2.4 Move the check in conf/YarpSystemCheck.cmake later
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag("-Wno-mismatched-tags" CXX_HAS_WNO_MISMATCHED_TAGS)
  if(CXX_HAS_WNO_MISMATCHED_TAGS)
    set_source_files_properties(${yarpmanager_gtk_SRCS} PROPERTIES COMPILE_FLAGS "-Wno-mismatched-tags")
  endif(CXX_HAS_WNO_MISMATCHED_TAGS)

  if(UNIX)
    add_definitions(-Wno-trigraphs)
  endif(UNIX)

  add_executable(yarpmanager-gtk ${yarpmanager_gtk_SRCS} ${yarpmanager_gtk_HDRS})
  target_link_libraries(yarpmanager-gtk YARP_init
                                        YARP_OS
                                        YARP_manager
                                        ${ACE_LIBRARIES}
                                        ${GTK2_GTK_LIBRARY}
                                        ${GTK2_GDK_LIBRARY}
                                        ${GTK2_GLIB_LIBRARY}
                                        ${GTK2_GTKMM_LIBRARY}
                                        ${GTK2_GLIBMM_LIBRARY}
                                        ${GTK2_GDKMM_LIBRARY}
                                        ${GTK2_ATKMM_LIBRARY}
                                        ${GTK2_SIGC++_LIBRARY}
                                        ${GTK2_GTHREAD_LIBRARY})

  install(TARGETS yarpmanager-gtk
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  if(NOT TARGET yarpmanager-qt OR YARP_DEFAULT_GTK)
    set_target_properties(yarpmanager-gtk PROPERTIES OUTPUT_NAME yarpmanager)
  endif()

endif()