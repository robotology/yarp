# SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_executable(yarpDeviceParamParserGenerator)
target_sources(yarpDeviceParamParserGenerator
  PRIVATE
    main.cpp
    generator.h
    generate_h_functions.cpp
    generate_cpp_functions.cpp
    generate_ini.cpp
    generate_md.cpp
    generate_yarprobotinterface.cpp
    generate_yarpdev.cpp
    parse_ini.cpp
    parse_md.cpp
    parse_extraComments.cpp
    paramGroupTree.h
    paramGroupTree.cpp
    parameter.h
    parameter.cpp
    utils.h
)
target_link_libraries(yarpDeviceParamParserGenerator
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
    YARP::YARP_init
)

install(
  TARGETS yarpDeviceParamParserGenerator
  COMPONENT utilities
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_property(TARGET yarpDeviceParamParserGenerator PROPERTY FOLDER "Command Line Tools")

if(YARP_COMPILE_TESTS)
  add_subdirectory(tests)
endif()
