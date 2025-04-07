# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fakeFrameWriter ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeFrameWriter
  CATEGORY device
  TYPE FakeFrameWriter
  INCLUDE FakeFrameWriter.h
  GENERATE_PARSER
  EXTRA_CONFIG
    WRAPPER=frameWriter_nws_yarp
)

if(ENABLE_fakeFrameWriter)
  yarp_add_plugin(yarp_fakeFrameWriter)

  target_sources(yarp_fakeFrameWriter
    PRIVATE
      FakeFrameWriter.cpp
      FakeFrameWriter.h
      FakeFrameWriter_ParamsParser.cpp
      FakeFrameWriter_ParamsParser.h
  )

  target_link_libraries(yarp_fakeFrameWriter
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_fakeFrameWriter
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeFrameWriter PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
