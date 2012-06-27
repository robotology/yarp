project(gPortScope)

cmake_minimum_required(VERSION 2.6.4)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})

find_package(YARP REQUIRED)
find_package(GtkMM REQUIRED)

include_directories(${YARP_INCLUDE_DIRS}
                    ${GTKMM_INCLUDE_DIRS}
)

set(USE_SYSTEM_GTKDATABOXMM OFF CACHE BOOL "Use system GtkDataboxMM library")
mark_as_advanced(USE_SYSTEM_GTKDATABOXMM)
if(USE_SYSTEM_GTKDATABOXMM)
    find_package(GtkDataboxMM REQUIRED)
    include_directories(${GTKDATABOXMM_INCLUDE_DIRS})
    link_directories(${GTKDATABOXMM_LIBRARY_DIRS})
    set(USE_SYSTEM_GTKDATABOX ON CACHE BOOL "Use system GtkDatabox library" FORCE)
else(USE_SYSTEM_GTKDATABOXMM)
    set(USE_SYSTEM_GTKDATABOX OFF CACHE BOOL "Use system GtkDatabox library")
    mark_as_advanced(USE_SYSTEM_GTKDATABOX)
    if(USE_SYSTEM_GTKDATABOX)
        find_package(GtkDatabox REQUIRED)
        include_directories(${GTKDATABOX_INCLUDE_DIRS})
        link_directories(${GTKDATABOX_LIBRARY_DIRS})
    endif(USE_SYSTEM_GTKDATABOX)
endif(USE_SYSTEM_GTKDATABOXMM)


add_definitions("-Wall --ansi")

add_subdirectory(src)
