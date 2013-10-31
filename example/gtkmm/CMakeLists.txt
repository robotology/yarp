# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giorgio Metta
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

PROJECT(gtkmmdemo)

SET(YARP_DIR "$ENV{YARP_ROOT}" CACHE LOCATION "where is yarp?")
SET(CMAKE_MODULE_PATH "$ENV{YARP_ROOT}/conf")

INCLUDE(UsePkgConfig)
FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(GtkMM REQUIRED)

ADD_DEFINITIONS(${GTKMM_C_FLAGS})
INCLUDE_DIRECTORIES(${GTKMM_INCLUDE_DIR})

FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.h include/*.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

ADD_EXECUTABLE(gtkmmdemo ${folder_source} ${folder_header})
install(TARGETS gtkmmdemo RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES basic.glade DESTINATION ${CMAKE_INSTALL_BINDIR})

TARGET_LINK_LIBRARIES(gtkmmdemo ${GTKMM_LINK_FLAGS})

IF(WIN32 AND NOT CYGWIN)
    SET_TARGET_PROPERTIES(gtkmmdemo PROPERTIES COMPILE_FLAGS /wd4250)
ENDIF(WIN32 AND NOT CYGWIN)
