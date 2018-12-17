set(SRC main.cpp)
add_executable(moduleLoader ${SRC})
target_link_libraries(moduleLoader YARP::YARP_OS
                                   YARP::YARP_init)