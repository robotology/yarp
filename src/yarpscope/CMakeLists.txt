project(gPortScope)

cmake_minimum_required(VERSION 2.6.4)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

find_package(YARP REQUIRED)
find_package(GtkMM REQUIRED)
find_package(GtkDataboxMM REQUIRED)

include_directories(${YARP_INCLUDE_DIRS}
                    ${GTKMM_INCLUDE_DIRS}
                    ${GTKDATABOXMM_INCLUDE_DIRS}
)

link_directories(${GTKDATABOXMM_LIBRARY_DIRS})

add_definitions("-Wall --ansi")

add_subdirectory(src)
