# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# BUILD_SHARED_LIBS is required
if (BUILD_SHARED_LIBS)
    include(YarpCatchUtils)

    add_executable(harness_pm_test_numbers)

    target_sources(harness_pm_test_numbers PRIVATE
      test_numbersTest.cpp
    )

    target_include_directories(harness_pm_test_numbers PRIVATE ${hmac_INCLUDE_DIRS})

    target_link_libraries(harness_pm_test_numbers
      PRIVATE
        YARP_harness
        YARP::YARP_os
        YARP::YARP_stats
    )

    set_property(TARGET harness_pm_test_numbers PROPERTY FOLDER "Test")

    yarp_catch_discover_tests(harness_pm_test_numbers)
endif()
