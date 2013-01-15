#  Yarp Application Builder
#  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
# 
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
 

GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

FIND_PACKAGE(GtkMM)
IF (NOT GtkMM_FOUND)
    MESSAGE(SEND_ERROR "GtkMM was not found but gyarpbuilder requires GtkMM.")
ENDIF (NOT GtkMM_FOUND)


FILE(GLOB folder_source src/*.cpp)
FILE(GLOB folder_header include/*.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

ADD_DEFINITIONS(${GtkMM_C_FLAGS})

INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${ACE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${TinyXML_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GtkMM_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GooCanvasMM_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(../libymanager/include)

# GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES

IF (WIN32 AND NOT CYGWIN)
    ADD_EXECUTABLE(gyarpbuilder ${folder_source} ${folder_header})
    TARGET_LINK_LIBRARIES(gyarpbuilder YARP_init YARP_OS YARP_manager ${GtkMM_LIBRARIES} ${GooCanvasMM_LIBRARIES})
ELSE (WIN32 AND NOT CYGWIN)
    LINK_DIRECTORIES(${GTKMM_LIBRARY_DIRS})
    ADD_EXECUTABLE(gyarpbuilder ${folder_source} ${folder_header})
    TARGET_LINK_LIBRARIES(gyarpbuilder YARP_init YARP_OS YARP_manager ${GtkMM_LIBRARIES} ${GooCanvasMM_LIBRARIES})
ENDIF (WIN32 AND NOT CYGWIN)

INSTALL(TARGETS gyarpbuilder COMPONENT utilities DESTINATION bin)



