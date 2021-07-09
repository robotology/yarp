# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(HDRS module.h)

yarp_prepare_plugin(myModuleBar
                    CATEGORY rfmodule
                    TYPE moduleBar
                    INCLUDE module.h)

if(ENABLE_myModules_myModuleBar)
  yarp_add_plugin(module_Bar)
  target_sources(module_Bar PRIVATE ${HDRS})
  target_link_libraries(module_Bar PRIVATE YARP::YARP_os)
endif()

#create the executable target
add_executable(myModuleBarExe)
target_sources(myModuleBarExe PRIVATE main.cpp ${SRC} ${HDRS})
target_link_libraries(myModuleBarExe PRIVATE YARP::YARP_os YARP::YARP_init)
