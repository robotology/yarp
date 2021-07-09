# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_executable(harness_sig)

target_sources(harness_sig PRIVATE ImageTest.cpp
                                   MatrixTest.cpp
                                   PointCloudTest.cpp
                                   SoundTest.cpp
                                   VectorOfTest.cpp
                                   VectorTest.cpp)

if (YARP_HAS_FFMPEG)
   target_compile_definitions(harness_sig PRIVATE YARP_MP3_SUPPORTED)
endif()

target_link_libraries(harness_sig PRIVATE YARP_harness
                                          YARP::YARP_os
                                          YARP::YARP_sig
                                          YARP::YARP_gsl)

set_property(TARGET harness_sig PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_sig)
