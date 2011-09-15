#  Yarp Modules Manager
#  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
# 
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
 

GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

IF (NOT UNIX)

	MESSAGE(STATUS "gyarpmanager is currently available only for linux distributions.")

ELSE (NOT UNIX)

	FILE(GLOB folder_source *.cpp)
    FILE(GLOB folder_header *.h)
	source_group("Source Files" FILES ${folder_source})
	source_group("Header Files" FILES ${folder_header})

	#PKG_CHECK_MODULES(GOOCANVASMM goocanvasmm-1.0)
	# GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES 
	#FIND_PACKAGE(PkgConfig)
	#PKG_CHECK_MODULES(GTKMM gtkmm-2.4)

	FIND_PACKAGE(GTK2 2.4 COMPONENTS gtk gtkmm REQUIRED)

	INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(${ACE_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES(../libymanager/include)
	INCLUDE_DIRECTORIES(../libymanager/tinyxml)
	INCLUDE_DIRECTORIES(${GTKMM_INCLUDE_DIRS})

	LINK_DIRECTORIES(${GTKMM_LIBRARY_DIRS})

	ADD_EXECUTABLE(gyarpmanager ${folder_source} ${folder_header} )
	TARGET_LINK_LIBRARIES(gyarpmanager YARP_init YARP_OS YARP_manager ${GTKMM_LIBRARIES})
	
	INSTALL(TARGETS gyarpmanager COMPONENT utilities DESTINATION bin)

ENDIF (NOT UNIX)


