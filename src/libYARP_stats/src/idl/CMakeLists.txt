# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpChooseIDL)
yarp_choose_idl(YARPSTATSMSG_THRIFT YarpStatsMsgs.thrift)

add_library(YarpStatsMsgs OBJECT)

target_sources(YarpStatsMsgs PRIVATE ${YARPSTATSMSG_THRIFT_GEN_FILES})

target_link_libraries(YarpStatsMsgs
  PRIVATE
    YARP::YARP_os
)

target_include_directories(YarpStatsMsgs PUBLIC ${YARPSTATSMSG_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET YarpStatsMsgs PROPERTY FOLDER "Libraries/Private")
