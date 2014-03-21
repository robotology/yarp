# Copyright: (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
# Authors: Vadim Tikhanoff
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

set(PROJECTNAME dataSetPlayer)
project(${PROJECTNAME})

# include the idl structure
include(YarpIDL)
yarp_idl_to_dir(${PROJECTNAME}.thrift ${IDL_GENERATED_CODE_DIR})

# Search for source code.
file(GLOB folder_source src/*.cpp src/*.cc src/*.c)
file(GLOB folder_header include/iCub/*.h include/*h)

list(APPEND folder_source ${IDL_GENERATED_CODE_DIR}/src/${PROJECTNAME}_IDL.cpp)
list(APPEND folder_header ${IDL_GENERATED_CODE_DIR}/include/${PROJECTNAME}_IDL.h)

source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

#link_directories(
#    ${GTKMM_LIBRARY_DIRS} )
	
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${IDL_GENERATED_CODE_DIR}/include
                    ${GTK2_INCLUDE_DIRS}
                    ${ICUB_INCLUDE_DIRS}
                    ${YARP_INCLUDE_DIRS}
                    ${OpenCV_INCLUDE_DIRS} )

add_executable(${PROJECTNAME} ${folder_header} ${folder_source})
target_link_libraries(${PROJECTNAME} ${YARP_LIBRARIES} ${OpenCV_LIBRARIES} ${GTK2_LIBRARIES})
install(TARGETS ${PROJECTNAME} DESTINATION bin)



