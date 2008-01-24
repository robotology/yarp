# make sure YARP_DIR environment variable is set
FIND_PACKAGE(YARP REQUIRED)

PROJECT(dev_check)

SET(USE_NEW_DEVICE_MECHANISM TRUE CACHE BOOL "use new method for devices")

IF (USE_NEW_DEVICE_MECHANISM)
  ADD_DEFINITIONS(-DUSE_NEW_DEVICE_MECHANISM)
  # link to the yarp drivers, and make an interface to them in "drivers.h"
  IMPORT_DEVICES(drivers.h yarpmod)
ENDIF (USE_NEW_DEVICE_MECHANISM)

ADD_EXECUTABLE(dev_check main.cpp)
