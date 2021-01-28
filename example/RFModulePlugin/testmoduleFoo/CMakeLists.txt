# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(HDRS module.h)

yarp_prepare_plugin(myModuleFoo
                    CATEGORY rfmodule
                    TYPE moduleFoo
                    INCLUDE module.h)

if(ENABLE_myModules_myModuleFoo)
  yarp_add_plugin(module_Foo)
  target_sources(module_Foo PRIVATE ${HDRS})
  target_link_libraries(module_Foo PRIVATE YARP::YARP_os)
endif()

#create the executable target
add_executable(myModuleFooExe)
target_sources(myModuleFooExe PRIVATE main.cpp ${SRC} ${HDRS})
target_link_libraries(myModuleFooExe PRIVATE YARP::YARP_os YARP::YARP_init)
