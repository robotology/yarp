# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(Demo)

add_executable(DemoServer)
target_sources(DemoServer PRIVATE DemoServer.cpp)
target_include_directories(DemoServer PRIVATE ${generated_libs_include_dir})
target_link_libraries(DemoServer PRIVATE ${YARP_LIBRARIES}
                                         generatedDemoLibrary)

add_executable(DemoServerExtended)
target_sources(DemoServerExtended PRIVATE DemoServerExtended.cpp)
target_include_directories(DemoServerExtended PRIVATE ${generated_libs_include_dir})
target_link_libraries(DemoServerExtended PRIVATE ${YARP_LIBRARIES}
                                                 generatedDemoLibrary)

add_executable(DemoServerModule)
target_sources(DemoServerModule PRIVATE DemoServerModule.cpp)
target_include_directories(DemoServerModule PRIVATE ${generated_libs_include_dir})
target_link_libraries(DemoServerModule PRIVATE ${YARP_LIBRARIES}
                                               generatedDemoLibrary)

add_executable(DemoClient)
target_sources(DemoClient PRIVATE DemoClient.cpp)
target_include_directories(DemoClient PRIVATE ${generated_libs_include_dir})
target_link_libraries(DemoClient PRIVATE ${YARP_LIBRARIES}
                                         generatedDemoLibrary)
