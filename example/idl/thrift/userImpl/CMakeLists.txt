# Copyright: (C) 2012 Robotics Brain and Cognitive Sciences, Istituto Italiano di Tecnologia
# Authors: Elena Ceseracciu
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(Demo)
include_directories(${generated_libs_include_dir})

add_executable(DemoServer DemoServer.cpp)
target_link_libraries(DemoServer ${YARP_LIBRARIES} generatedDemoLibrary)

add_executable(DemoServerExtended DemoServerExtended.cpp)
target_link_libraries(DemoServerExtended ${YARP_LIBRARIES} generatedDemoLibrary)

add_executable(DemoServerModule DemoServerModule.cpp)
target_link_libraries(DemoServerModule ${YARP_LIBRARIES} generatedDemoLibrary)

add_executable(DemoClient DemoClient.cpp)
target_link_libraries(DemoClient ${YARP_LIBRARIES} generatedDemoLibrary)

