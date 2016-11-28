#  Yarp Modules Manager
#  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(CREATE_YARPMANAGER_CONSOLE)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_manager_INCLUDE_DIRS TARGET YARP_manager PROPERTY INCLUDE_DIRS)


  file(GLOB folder_source *.cpp)
  file(GLOB folder_header *.h)
  source_group("Source Files" FILES ${folder_source})
  source_group("Header Files" FILES ${folder_header})

  include_directories(${YARP_OS_INCLUDE_DIRS})
  include_directories(${YARP_manager_INCLUDE_DIRS})
  include_directories(${ACE_INCLUDE_DIRS})

  if (YARP_HAS_LIBEDIT)
    add_definitions( -DWITH_LIBEDIT )
    include_directories(${Libedit_INCLUDE_DIR})
  endif()

  add_executable(yarpmanager-console ${folder_source} ${folder_header} )

  target_link_libraries(yarpmanager-console YARP_init
                                            YARP_OS
                                            YARP_manager
                                            ${ACE_LIBRARIES})

  if(YARP_HAS_LIBEDIT)
    target_link_libraries(yarpmanager-console Libedit::libedit)
  endif()

  install(TARGETS yarpmanager-console
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()