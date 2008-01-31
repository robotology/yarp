# make sure YARP_DIR environment variable is set
FIND_PACKAGE(YARP REQUIRED)

PROJECT(dev_check)

ADD_EXECUTABLE(dev_check main.cpp)
