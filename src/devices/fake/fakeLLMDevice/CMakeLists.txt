# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
    set(ENABLE_yarpmod_fakeLLMDevice ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeLLMDevice
  CATEGORY device
  TYPE FakeLLMDevice
  INCLUDE FakeLLMDevice.h
  GENERATE_PARSER
)

if(NOT SKIP_fakeLLMDevice)
    yarp_add_plugin(yarp_fakeLLMDevice)

    target_sources(yarp_fakeLLMDevice
    PRIVATE
        FakeLLMDevice.cpp
        FakeLLMDevice.h
        FakeLLMDevice_ParamsParser.cpp
        FakeLLMDevice_ParamsParser.h
    )

    target_link_libraries(yarp_fakeLLMDevice
    PRIVATE
        YARP::YARP_os
        YARP::YARP_dev
    )
    list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
        YARP_os
        YARP_dev
    )

    yarp_install(
        TARGETS yarp_fakeLLMDevice
        EXPORT YARP_${YARP_PLUGIN_MASTER}
        COMPONENT ${YARP_PLUGIN_MASTER}
        LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
        ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
        YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
    )
    set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

    set_property(TARGET yarp_fakeLLMDevice PROPERTY FOLDER "Plugins/Device/Fake")

    if(YARP_COMPILE_TESTS)
        add_subdirectory(tests)
    endif()

endif()
