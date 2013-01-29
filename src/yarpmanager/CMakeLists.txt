#  Yarp Modules Manager
#  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
# 
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

# yarpmanager 

if (CREATE_YMANAGER)
	add_subdirectory(libymanager)
	add_subdirectory(ymanager)	
	if (CREATE_GUIS)
#		option(CREATE_YMANAGER_GUI "Do you want to compile GUI for Yarp module manager?" OFF)
#		if(CREATE_YMANAGER_GUI)
			add_subdirectory(gymanager)
#		endif()
	endif (CREATE_GUIS)
endif (CREATE_YMANAGER)

if (CREATE_YBUILDER)
	add_subdirectory(gybuilder)
endif (CREATE_YBUILDER)


