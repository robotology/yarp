# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# BUILD_SHARED_LIBS is required
if (BUILD_SHARED_LIBS)
    include(YarpCatchUtils)

    add_executable(harness_pm_image_compression_ffmpeg)

    target_sources(harness_pm_image_compression_ffmpeg PRIVATE
      image_compression_ffmpegTest.cpp
    )

    target_include_directories(harness_pm_image_compression_ffmpeg PRIVATE ${hmac_INCLUDE_DIRS})

    target_link_libraries(harness_pm_image_compression_ffmpeg
      PRIVATE
        YARP_harness
        YARP::YARP_os
        YARP::YARP_sig
    )

    set_property(TARGET harness_pm_image_compression_ffmpeg PROPERTY FOLDER "Test")

    yarp_catch_discover_tests(harness_pm_image_compression_ffmpeg)
endif()
