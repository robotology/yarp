#  Yarp Modules Manager
#  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
# 
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
 


GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

# IF (NOT UNIX)

#	MESSAGE(STATUS "yarpmanager is currently available only for linux distributions.")

# ELSE (NOT UNIX)

	FILE(GLOB folder_source *.cpp)
    FILE(GLOB folder_header *.h)
	source_group("Source Files" FILES ${folder_source})
	source_group("Header Files" FILES ${folder_header})

	INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(${ACE_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(../libymanager/include)
	INCLUDE_DIRECTORIES(../libymanager/tinyxml)

	if (YARP_USE_READLINE)
		#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -DWITH_READLINE")
		ADD_DEFINITIONS( -DWITH_READLINE )
		include_directories(${READLINE_INCLUDE_DIR})
	else ()
		MESSAGE(STATUS "yarpmanager will use basic shell mode instead of GNU readline.")
	endif ()

	ADD_EXECUTABLE(yarpmanager ${folder_source} ${folder_header} )
	IF (READLINE_FOUND)
		TARGET_LINK_LIBRARIES(yarpmanager YARP_init YARP_OS YARP_manager ${ACE_LIBRARIES} ${READLINE_LIBRARY})
	ELSE (READLINE_FOUND)
		TARGET_LINK_LIBRARIES(yarpmanager YARP_init YARP_OS YARP_manager ${ACE_LIBRARIES})
	ENDIF (READLINE_FOUND)

	INSTALL(TARGETS yarpmanager COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

# ENDIF (NOT UNIX)


