# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# compile if needed - used by tcpros_carrier, mjpeg_carrier

if(ENABLE_yarpcar_mjpeg_carrier OR ENABLE_yarpcar_tcpros_carrier)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    include_directories(${ACE_INCLUDE_DIR})
    get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
    include_directories(${YARP_INCLUDE_DIRS})
    add_library(YARP_wire_rep_utils STATIC WireImage.cpp
                                           WireImage.h
                                           WireBottle.cpp
                                           WireBottle.h
                                           WireTwiddler.cpp
                                           WireTwiddler.h)
    target_link_libraries(YARP_wire_rep_utils LINK_PRIVATE YARP_OS YARP_sig)
    target_link_libraries(YARP_wire_rep_utils LINK_PRIVATE ${ACE_LIBRARIES})
    if(UNIX)
        set_target_properties(YARP_wire_rep_utils PROPERTIES COMPILE_FLAGS -fPIC)
    endif(UNIX)

    set_property(TARGET YARP_wire_rep_utils PROPERTY INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
    set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_wire_rep_utils)
    install(TARGETS YARP_wire_rep_utils EXPORT YARP COMPONENT runtime DESTINATION ${CMAKE_INSTALL_LIBDIR})

    if(YARP_COMPILE_TESTS)
        add_executable(wire_rep_test test.cpp)
        target_link_libraries(wire_rep_test YARP_wire_rep_utils)
        target_link_libraries(wire_rep_test YARP_OS YARP_sig YARP_init)
        target_link_libraries(wire_rep_test ${ACE_LIBRARIES})
   endif(YARP_COMPILE_TESTS)

endif(ENABLE_yarpcar_mjpeg_carrier OR ENABLE_yarpcar_tcpros_carrier)
