# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(wxsdl TYPE yarp::dev::WxsdlWriter INCLUDE WxsdlWriter.h)
  YARP_ADD_DEVICE_FINGERPRINT(wxsdl.ini wxsdl)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_wxsdl)

  # make a stand-alone test executable

  FIND_PACKAGE(YARP REQUIRED)
  FIND_PACKAGE(wxWidgets REQUIRED)
  FIND_PACKAGE(SDL REQUIRED)
 
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${wxWidgets_INCLUDE_DIRS})
  MESSAGE(STATUS ">>> ${wxWidgets_DEFINITIONS}")
  # ADD_DEFINITIONS(${wxWidgets_DEFINITIONS})
  LINK_LIBRARIES(${wxWidgets_LIBRARIES})

  INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
  LINK_LIBRARIES(${SDL_LIBRARY})
  IF (WIN32)
     LINK_LIBRARIES(winmm)
  ENDIF (WIN32)

  YARP_ADD_PLUGIN(yarp_wxsdl WxsdlWriter.cpp  WxsdlWriter.h)
  SET_TARGET_PROPERTIES(yarp_wxsdl PROPERTIES COMPILE_DEFINITIONS "${wxWidgets_DEFINITIONS}")

  IF (NOT COMPILE_DEVICE_LIBRARY)
    ADD_DEFINITIONS(-DWXSDL_MAIN)
    ADD_EXECUTABLE(wxsdl_test  WxsdlWriter.cpp  WxsdlWriter.h)
  ENDIF (NOT COMPILE_DEVICE_LIBRARY)

ENDIF(NOT SKIP_wxsdl)
