#  Yarp Modules Manager
#  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
# 
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
 

GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

IF (APPLE)

	MESSAGE(STATUS "gyarpmanager is currently available only for Windows and Linux distributions.")

ELSE (APPLE)

	FIND_PACKAGE(GtkMM)
	IF (NOT GtkMM_FOUND)
		MESSAGE(SEND_ERROR "GtkMM was not found but gyarpmanager requires GtkMM.")
	ENDIF (NOT GtkMM_FOUND)

	FILE(GLOB folder_source *.cpp)
    FILE(GLOB folder_header *.h)
	source_group("Source Files" FILES ${folder_source})
	source_group("Header Files" FILES ${folder_header})

	ADD_DEFINITIONS(${GtkMM_C_FLAGS})
		
	INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(${ACE_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(../libymanager/include)
	INCLUDE_DIRECTORIES(../libymanager/tinyxml)
	INCLUDE_DIRECTORIES(${GtkMM_INCLUDE_DIRS})

	#PKG_CHECK_MODULES(GOOCANVASMM goocanvasmm-1.0)
	# GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES 

	IF (WIN32 AND NOT CYGWIN)
        #ADD_EXECUTABLE(gyarpmanager WIN32 ${folder_source} ${folder_header})
        ADD_EXECUTABLE(gyarpmanager ${folder_source} ${folder_header})
		TARGET_LINK_LIBRARIES(gyarpmanager YARP_init YARP_OS YARP_manager ${GtkMM_LIBRARIES})
	ELSE (WIN32 AND NOT CYGWIN)
		ADD_EXECUTABLE(gyarpmanager ${folder_source} ${folder_header} )
		TARGET_LINK_LIBRARIES(gyarpmanager YARP_init YARP_OS YARP_manager ${GtkMM_LIBRARIES})
	ENDIF (WIN32 AND NOT CYGWIN)
	
	INSTALL(TARGETS gyarpmanager COMPONENT utilities DESTINATION bin)

ENDIF (APPLE)


