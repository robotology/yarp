cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
 cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)



PROJECT (stressTests)

FIND_PACKAGE(YARP)

ADD_EXECUTABLE(stressrpc stressrpc.cpp)
