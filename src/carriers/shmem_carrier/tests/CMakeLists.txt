# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_executable(harness_carrier_shmem)
target_sources(harness_carrier_shmem
  PRIVATE
    shmem.cpp
)

target_link_libraries(harness_carrier_shmem
  PRIVATE
    YARP_harness
    YARP::YARP_os
)

set_property(TARGET harness_carrier_shmem PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_carrier_shmem)
