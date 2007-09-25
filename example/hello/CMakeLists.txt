
# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

FIND_PACKAGE(YARP)

PROJECT(example)

ADD_EXECUTABLE(example main.cpp)
