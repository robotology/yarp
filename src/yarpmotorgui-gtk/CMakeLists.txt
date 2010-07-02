SET(PROJECTNAME robotMotorGui)
PROJECT(${PROJECTNAME})

FILE(GLOB_RECURSE folder_header src/include/*.h)
FILE(GLOB_RECURSE folder_source src/*.cpp)
SOURCE_GROUP("Header Files" FILES ${folder_header})
SOURCE_GROUP("Source Files" FILES ${folder_source})

ADD_DEFINITIONS(${GTKPLUS_C_FLAGS})

INCLUDE_DIRECTORIES(${ctrlLib_INCLUDE_DIRS}
                      ${GtkPlus_INCLUDE_DIRS}
					  ${YARP_INCLUDE_DIRS})

ADD_EXECUTABLE(${PROJECTNAME} ${folder_header} ${folder_source})

TARGET_LINK_LIBRARIES(${PROJECTNAME} ctrlLib
										icubmod
										${GtkPlus_LIBRARIES}
										${YARP_LIBRARIES})

INSTALL(TARGETS ${PROJECTNAME} DESTINATION bin)

