
# set up to use libYARP_*
SET(NEED_LIBS YARP_OS YARP_sig YARP_dev)

LINK_LIBRARIES(${YARP_dev_EXT_LIBS} YARP_OS YARP_sig YARP_dev)

YarpProgram(yarpdev NO_CONSOLE) #damn!
INSTALL_TARGETS(/bin yarpdev)
