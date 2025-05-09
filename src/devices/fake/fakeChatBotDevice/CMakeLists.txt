# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
    set(ENABLE_yarpmod_fakeChatBotDevice ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeChatBotDevice
  CATEGORY device
  TYPE FakeChatBotDevice
  INCLUDE FakeChatBotDevice.h
  GENERATE_PARSER
)

if(NOT SKIP_fakeChatBotDevice)

    yarp_add_plugin(yarp_fakeChatBotDevice)

    target_sources(yarp_fakeChatBotDevice
    PRIVATE
        FakeChatBotDevice.cpp
        FakeChatBotDevice.h
        FakeChatBotDevice_ParamsParser.cpp
        FakeChatBotDevice_ParamsParser.h
    )

    target_link_libraries(yarp_fakeChatBotDevice
    PRIVATE
        YARP::YARP_os
        YARP::YARP_dev
    )
    list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
        YARP_os
        YARP_dev
    )

    yarp_install(
        TARGETS yarp_fakeChatBotDevice
        EXPORT YARP_${YARP_PLUGIN_MASTER}
        COMPONENT ${YARP_PLUGIN_MASTER}
        LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
        ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
        YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
    )
    set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

    set_property(TARGET yarp_fakeChatBotDevice PROPERTY FOLDER "Plugins/Device/Fake")

    if(YARP_COMPILE_TESTS)
        add_subdirectory(tests)
    endif()

endif()
