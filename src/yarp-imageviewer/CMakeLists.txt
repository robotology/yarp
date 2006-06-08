# set up to use GTK+

ADD_DEFINITIONS(${GTKPLUS_C_FLAGS})
LINK_LIBRARIES(${GTKPLUS_LINK_FLAGS})

INCLUDE_DIRECTORIES(${GTKPLUS_INCLUDE_DIR})

# set up to use libYARP_*
SET(NEED_LIBS YARP_OS YARP_sig)

IF(WIN32)
	YarpProgram(yarpimageviewer NO_CONSOLE) #damn!
	INSTALL_TARGETS(/bin yarpimageviewer)
ELSE(WIN32)
	YarpProgram(yarp-imageviewer CONSOLE)
ENDIF(WIN32)

