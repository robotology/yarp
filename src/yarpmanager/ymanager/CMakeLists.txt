#  Yarp Modules Manager
#  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)


file(GLOB folder_source *.cpp)
file(GLOB folder_header *.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

include_directories(${YARP_OS_INCLUDE_DIRS})
include_directories(${ACE_INCLUDE_DIRS})
include_directories(../libymanager/include)

if (YARP_USE_READLINE)
    add_definitions( -DWITH_READLINE )
    include_directories(${Readline_INCLUDE_DIR})
else()
    message(STATUS "yarpmanager will use basic shell mode instead of GNU readline.")
endif()

add_executable(yarpmanager ${folder_source} ${folder_header} )

if(YARP_USE_READLINE)
    target_link_libraries(yarpmanager YARP_init YARP_OS YARP_manager ${ACE_LIBRARIES} ${Readline_LIBRARY})
else()
    target_link_libraries(yarpmanager YARP_init YARP_OS YARP_manager ${ACE_LIBRARIES})
endif()

install(TARGETS yarpmanager COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
