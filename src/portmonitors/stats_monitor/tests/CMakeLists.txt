# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# BUILD_SHARED_LIBS is required
if (BUILD_SHARED_LIBS)
    include(YarpCatchUtils)

    add_executable(harness_pm_stats_monitor)

    target_sources(harness_pm_stats_monitor PRIVATE
      stats_monitorTest.cpp
    )

    target_include_directories(harness_pm_stats_monitor PRIVATE ${hmac_INCLUDE_DIRS})

    target_link_libraries(harness_pm_stats_monitor
      PRIVATE
        YARP_harness
        YARP::YARP_os
        YARP::YARP_stats
    )

    set_property(TARGET harness_pm_stats_monitor PROPERTY FOLDER "Test")

    yarp_catch_discover_tests(harness_pm_stats_monitor)
endif()
