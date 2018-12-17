################################################################################
#                                                                              #
# Copyright (C) 2018 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SRC module.cpp)
set(HDRS module.h)

yarp_prepare_plugin(myModuleFoo
                    CATEGORY rfmodule
                    TYPE moduleFoo
                    INCLUDE module.h)

if(ENABLE_myModules_myModuleFoo)
  yarp_add_plugin(module_Foo ${SRC} ${HDRS})
  target_link_libraries(module_Foo YARP::YARP_OS)
endif()

#create the executable target
add_executable(myModuleFooExe main.cpp ${SRC} ${HDRS})
target_link_libraries(myModuleFooExe YARP::YARP_OS YARP::YARP_init)
