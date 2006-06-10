
# You'll need to tell CMake where to find YARP.
# From the command line, you can do:
#   cmake -DYARP_DIR:LOCATION=../.. .
# where ../.. is replaced with the path to which you built YARP.

FIND_PACKAGE(YARP)

PROJECT(dev_check)

ADD_EXECUTABLE(dev_check main.cpp)
