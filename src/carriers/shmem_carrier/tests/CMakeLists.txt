# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# Helper executable, used to run the other side of the connection in a
# different process
add_executable(harness_carrier_shmem_peer)
target_sources(harness_carrier_shmem_peer
  PRIVATE
    shmem_peer.cpp
)

target_link_libraries(harness_carrier_shmem_peer
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)

set_property(TARGET harness_carrier_shmem_peer PROPERTY FOLDER "Test")

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

target_compile_definitions(harness_carrier_shmem
  PRIVATE
    SHMEM_PEER_PATH="$<TARGET_FILE:harness_carrier_shmem_peer>"
)
add_dependencies(harness_carrier_shmem harness_carrier_shmem_peer)

set_property(TARGET harness_carrier_shmem PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_carrier_shmem)
