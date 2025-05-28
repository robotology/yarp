# SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpPlugin)
include(YarpPrintFeature)
include(YarpCatchUtils)
include(YarpDeviceParamsParserGenerator)

yarp_prepare_plugin(testDeviceWGP2
  CATEGORY device
  TYPE TestDeviceWGP2
  INCLUDE TestDeviceWGP2.h
  DEFAULT ON
)

#This is a test so we always want to generate the parser, regardless the value of ALLOW_DEVICE_PARAM_PARSER_GENERATION.
#With add_custom_target and add_dependencies we create a dependency so that first yarpDeviceParamParserGenerator is executed
#then the device is compiled, using the generated files.
add_custom_target(generate_parser2 COMMAND yarpDeviceParamParserGenerator
                                          --class_name TestDeviceWGP2
                                          --module_name testDeviceWGP2
                                          --input_filename_md test.txt
                                          --input_extra_comments extraComments.md
                                          --generate_all
                                          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

yarp_add_plugin(yarp_testDeviceWGP2)

add_dependencies(yarp_testDeviceWGP2 generate_parser2)

target_sources(yarp_testDeviceWGP2
    PRIVATE
      TestDeviceWGP2.cpp
      TestDeviceWGP2.h
      TestDeviceWGP2_ParamsParser.cpp
      TestDeviceWGP2_ParamsParser.h
)

target_link_libraries(yarp_testDeviceWGP2
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
)
list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
)

  yarp_install(
    TARGETS yarp_testDeviceWGP2
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

source_group("OtherFiles" FILES
      testDeviceWGP2Params.ini
      testDeviceWGP2Params.md
      testDeviceWGP2Params.xml
)

set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_testDeviceWGP2 PROPERTY FOLDER "Test")
set_property(TARGET generate_parser2 PROPERTY FOLDER "Test")

create_device_test(TestDeviceWGP2)
