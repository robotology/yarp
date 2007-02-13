
PROJECT(yarp-motorcontrolgui)

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

ADD_EXECUTABLE(yarpmotorcontrol ${folder_source} ${folder_header})
INSTALL (TARGETS yarpmotorcontrol RUNTIME DESTINATION bin)
INSTALL (FILES yarpmotorcontrol.glade DESTINATION bin)

TARGET_LINK_LIBRARIES(yarpmotorcontrol ${GTKMM_LINK_FLAGS})
