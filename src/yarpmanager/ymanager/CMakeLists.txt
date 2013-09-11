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


	FIND_PATH(READLINE_INCLUDE_DIR readline/readline.h)
	FIND_LIBRARY(READLINE_LIBRARY NAMES readline)

	IF (READLINE_INCLUDE_DIR AND READLINE_LIBRARY)
		SET(READLINE_FOUND TRUE)
	ENDIF (READLINE_INCLUDE_DIR AND READLINE_LIBRARY)

	IF (READLINE_FOUND)
		#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -DWITH_READLINE")
		ADD_DEFINITIONS( -DWITH_READLINE )
		include_directories(${READLINE_INCLUDE_DIR})
		IF (NOT Readline_FIND_QUIETLY)
			MESSAGE(STATUS "Found GNU readline: ${READLINE_LIBRARY}")
		ENDIF (NOT Readline_FIND_QUIETLY)
	ELSE (READLINE_FOUND)
		MESSAGE(STATUS "Could not find GNU readline. yarpmanager will use basic shell mode.")
	ENDIF (READLINE_FOUND)

	ADD_EXECUTABLE(yarpmanager ${folder_source} ${folder_header} )
	IF (READLINE_FOUND)
		TARGET_LINK_LIBRARIES(yarpmanager YARP_init YARP_OS YARP_manager ${ACE_LIBRARIES} ${READLINE_LIBRARY})
	ELSE (READLINE_FOUND)
		TARGET_LINK_LIBRARIES(yarpmanager YARP_init YARP_OS YARP_manager ${ACE_LIBRARIES})
	ENDIF (READLINE_FOUND)

	INSTALL(TARGETS yarpmanager COMPONENT utilities DESTINATION bin)

# ENDIF (NOT UNIX)


