
# set up to use GTK+
ADD_DEFINITIONS(${GTKPLUS_C_FLAGS})
LINK_LIBRARIES(${GTKPLUS_LINK_FLAGS})

# set up to use libYARP_*
SET(NEED_LIBS YARP_OS YARP_sig)

YarpProgram(yarp-imageviewer)



