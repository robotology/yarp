# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# BUILD_SHARED_LIBS is required
if (BUILD_SHARED_LIBS)
    include(YarpCatchUtils)

    add_executable(harness_pm_simulated_network_delay)

    target_sources(harness_pm_simulated_network_delay PRIVATE
      simulated_network_delayTest.cpp
    )

    target_include_directories(harness_pm_simulated_network_delay PRIVATE ${hmac_INCLUDE_DIRS})

    target_link_libraries(harness_pm_simulated_network_delay
      PRIVATE
        YARP_harness
        YARP::YARP_os
    )

    set_property(TARGET harness_pm_simulated_network_delay PROPERTY FOLDER "Test")

    yarp_catch_discover_tests(harness_pm_simulated_network_delay)
endif()
