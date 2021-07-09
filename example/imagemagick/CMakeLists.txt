# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
project(magicktest)

find_package(YARP COMPONENTS sig REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)

add_executable(magicktest)
target_sources(magicktest PRIVATE main.cpp)

target_link_libraries(magicktest PRIVATE ${YARP_LIBRARIES})

target_include_directories(magicktest PRIVATE ${ImageMagick_INCLUDE_DIRS})
target_link_libraries(magicktest PRIVATE ${ImageMagick_LIBRARIES})
