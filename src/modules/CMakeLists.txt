
PROJECT(YarpModules)

MESSAGE(STATUS "This is an experimental way to compile devices")

FIND_PACKAGE(YARP REQUIRED)

BEGIN_DEVICE_LIBRARY(mydevices MERGE) # omit "MERGE" if get clashes

#ADD_SUBDIRECTORY(cuda)
ADD_SUBDIRECTORY(fakebot)
#ADD_SUBDIRECTORY(nvidia)
ADD_SUBDIRECTORY(stage)
ADD_SUBDIRECTORY(vfw)
ADD_SUBDIRECTORY(wxsdl)

END_DEVICE_LIBRARY(mydevices)

