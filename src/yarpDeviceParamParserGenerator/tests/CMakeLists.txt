# SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpPlugin)
include(YarpPrintFeature)
include(YarpCatchUtils)
include(YarpDeviceParamsGenerator)

yarp_prepare_plugin(testDeviceWGP
  CATEGORY device
  TYPE TestDeviceWGP
  INCLUDE TestDeviceWGP.h
  DEFAULT ON
)

#This is a test so we always want to generate the parser, regardless the value of ALLOW_DEVICE_PARAM_PARSER_GENERATION.
#With add_custom_target and add_dependencies we create a dependency so that first yarpDeviceParamParserGenerator is executed
#then the device is compiled, using the generated files.
add_custom_target(generate_parser COMMAND yarpDeviceParamParserGenerator
                                          --class_name TestDeviceWGP
                                          --input_filename_md test2.txt
                                          --input_extra_comments extraComments.md
                                          --generate all
                                          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

yarp_add_plugin(yarp_testDeviceWGP)

add_dependencies(yarp_testDeviceWGP generate_parser)

target_sources(yarp_testDeviceWGP
    PRIVATE
      TestDeviceWGP.cpp
      TestDeviceWGP.h
      TestDeviceWGPParams.cpp
      TestDeviceWGPParams.h
)

target_link_libraries(yarp_testDeviceWGP
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
)
list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
)

  yarp_install(
    TARGETS yarp_testDeviceWGP
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

source_group("OtherFiles" FILES
      testDeviceWGPParams.ini
      testDeviceWGPParams.md
      testDeviceWGPParams.xml
)

set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_testDeviceWGP PROPERTY FOLDER "Test")

create_device_test(TestDeviceWGP)
