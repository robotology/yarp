# Copyright: (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
# Authors: Vadim Tikhanoff
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET(PROJECTNAME dataSetPlayer)
PROJECT(${PROJECTNAME})

# Search for source code.
file(GLOB folder_source src/*.cpp src/*.cc src/*.c)
file(GLOB folder_header include/iCub/*.h)

source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

link_directories(
    ${GTKMM_LIBRARY_DIRS} )
	
include_directories(${PROJECT_SOURCE_DIR}/include
					${GTKMM_INCLUDE_DIRS}
                    ${ICUB_INCLUDE_DIRS}
                    ${YARP_INCLUDE_DIRS}
					${OpenCV_INCLUDE_DIRS} )

add_executable(${PROJECTNAME} ${folder_header} ${folder_source})
target_link_libraries(${PROJECTNAME} ${YARP_LIBRARIES} ${OpenCV_LIBRARIES} ${GTKMM_LIBRARIES} ${GOOCANVASMM_LIBRARIES} )
install(TARGETS ${PROJECTNAME} DESTINATION bin)



