# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# BUILD_SHARED_LIBS is required
if (BUILD_SHARED_LIBS)
    include(YarpCatchUtils)

    add_executable(harness_pm_depthimage_compression_zlib)

    target_sources(harness_pm_depthimage_compression_zlib PRIVATE
      depthimage_compression_zlibTest.cpp
    )

    target_link_libraries(harness_pm_depthimage_compression_zlib
      PRIVATE
        YARP_harness
        YARP::YARP_os
        YARP::YARP_sig
    )

    set_property(TARGET harness_pm_depthimage_compression_zlib PROPERTY FOLDER "Test")

    yarp_catch_discover_tests(harness_pm_depthimage_compression_zlib)
endif()
