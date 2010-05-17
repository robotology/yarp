SET(PROJECTNAME dataDumper)

PROJECT(${PROJECTNAME})

IF(ICUB_USE_OpenCV)
   ADD_DEFINITIONS(-DADD_VIDEO)
   INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
ELSE(ICUB_USE_OpenCV)
   message (STATUS "${PROJECTNAME}: OpenCV not selected, keep on building...")
ENDIF(ICUB_USE_OpenCV)

SET(folder_source main.cpp)

SOURCE_GROUP("Source Files" FILES ${folder_source})

include_directories(${YARP_INCLUDE_DIRS})
ADD_EXECUTABLE(${PROJECTNAME} ${folder_source})

if (ICUB_USE_OpenCV)
	target_link_libraries(${PROJECTNAME} ${OpenCV_LIBRARIES})
endif (ICUB_USE_OpenCV)

TARGET_LINK_LIBRARIES(${PROJECTNAME} ${YARP_LIBRARIES})
INSTALL(TARGETS ${PROJECTNAME} DESTINATION bin)


