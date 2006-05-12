
SET(NEED_LIBS "")
YarpLibrary(YARP_OS os)

# add YARP Network companion executable
ADD_EXECUTABLE(yarp yarp/yarp.cpp)
TARGET_LINK_LIBRARIES(yarp YARP_OS)
INSTALL_TARGETS(/bin yarp)
