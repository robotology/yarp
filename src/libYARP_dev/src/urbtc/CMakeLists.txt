FIND_PACKAGE(YARP REQUIRED)

IF (UNIX)
#  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common ${ACE_INCLUDE_DIR})
#  LINK_LIBRARIES(${ACE_LIBRARY})

  IF (COMPILE_DEVICE_LIBRARY)
    PREPARE_DEVICE(urbtc TYPE UrbtcControl INCLUDE linux/yarp/UrbtcControl.h
			 WRAPPER controlboard)
  ENDIF (COMPILE_DEVICE_LIBRARY)

  ADD_LIBRARY(urbtc linux/yarp/UrbtcControl.cpp linux/yarp/UrbtcControl.h)

ELSE (UNIX)

  MESSAGE(STATUS "urbtc is on unix")

ENDIF (UNIX)


