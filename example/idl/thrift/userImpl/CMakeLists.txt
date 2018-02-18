# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

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

