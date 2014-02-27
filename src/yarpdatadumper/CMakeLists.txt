# Copyright: (C) 2010 RobotCub Consortium
# Authors: Ugo Pattacini
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

set(PROJECTNAME dataDumper)
project(${PROJECTNAME})

if(ICUB_USE_OpenCV)
   add_definitions(-DADD_VIDEO)
   include_directories(${OpenCV_INCLUDE_DIRS})
else(ICUB_USE_OpenCV)
   message(STATUS "${PROJECTNAME}: OpenCV not selected, keep on building...")
endif(ICUB_USE_OpenCV)

set(folder_source main.cpp)
source_group("Source Files" FILES ${folder_source})

include_directories(${YARP_INCLUDE_DIRS})
add_executable(${PROJECTNAME} ${folder_source})

if(ICUB_USE_OpenCV)
   target_link_libraries(${PROJECTNAME} ${OpenCV_LIBRARIES})
endif(ICUB_USE_OpenCV)

target_link_libraries(${PROJECTNAME} ${YARP_LIBRARIES})
install(TARGETS ${PROJECTNAME} DESTINATION bin)


