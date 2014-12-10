# Copyright: (C) 2012 Robotics Brain and Cognitive Sciences, Istituto Italiano di Tecnologia
# Authors: Elena Ceseracciu
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

SET(PROJECTNAME Demo)
PROJECT(${PROJECTNAME})
include_directories(${YARP_INCLUDE_DIRS} ${generated_libs_include_dir})

ADD_EXECUTABLE (${PROJECTNAME}Server DemoServer.cpp)
TARGET_LINK_LIBRARIES(${PROJECTNAME}Server ${YARP_LIBRARIES} generatedDemoLibrary)

ADD_EXECUTABLE (${PROJECTNAME}ServerExtended DemoServerExtended.cpp)
TARGET_LINK_LIBRARIES(${PROJECTNAME}ServerExtended ${YARP_LIBRARIES} generatedDemoLibrary)

ADD_EXECUTABLE (${PROJECTNAME}ServerModule DemoServerModule.cpp)
TARGET_LINK_LIBRARIES(${PROJECTNAME}ServerModule ${YARP_LIBRARIES} generatedDemoLibrary)

ADD_EXECUTABLE (${PROJECTNAME}Client DemoClient.cpp)
TARGET_LINK_LIBRARIES(${PROJECTNAME}Client ${YARP_LIBRARIES} generatedDemoLibrary)

