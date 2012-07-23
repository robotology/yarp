# Copyright (C) 2009  RobotCub Consortium
# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Lorenzo Natale, Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


macro(checkandset_dependency package)

    if (${package}_FOUND)
        set(YARP_HAS_${package} TRUE CACHE BOOL "Package ${package} found" FORCE)
        set(YARP_USE_${package} TRUE CACHE BOOL "Use package ${package}")
    else (${package}_FOUND)
        set(YARP_HAS_${package} FALSE CACHE BOOL "" FORCE)
        set(YARP_USE_${package} FALSE CACHE BOOL "Use package ${package}")
    endif (${package}_FOUND)

    set(YARP_USE_SYSTEM_${package} TRUE CACHE INTERNAL "" FORCE)

    mark_as_advanced(YARP_HAS_${package} YARP_USE_${package})

    #store all dependency flags for later export
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_USE_${package})

endmacro (checkandset_dependency)

macro(checkbuildandset_dependency package)

    if (${package}_FOUND)
        set(YARP_HAS_${package} TRUE CACHE BOOL "Package ${package} found" FORCE)
        set(YARP_USE_${package} TRUE CACHE BOOL "Use package ${package}")
        set(YARP_USE_SYSTEM_${package} TRUE CACHE BOOL "Use system-installed ${package}, rather than a private copy")
    else (${package}_FOUND)
        # If package was not found we force it to be built
        set(YARP_USE_SYSTEM_${package} FALSE CACHE BOOL "Use system-installed ${package}, rather than a private copy" FORCE)
        if(NOT YARP_USE_SYSTEM_${package})
            set(YARP_HAS_${package} TRUE CACHE BOOL "" FORCE)
            set(YARP_USE_${package} TRUE CACHE BOOL "Use package ${package}")
        else(NOT YARP_USE_SYSTEM_${package})
            set(YARP_HAS_${package} FALSE CACHE BOOL "" FORCE)
            set(YARP_USE_${package} FALSE CACHE BOOL "Use package ${package}")
        endif(NOT YARP_USE_SYSTEM_${package})
    endif (${package}_FOUND)

    mark_as_advanced(YARP_HAS_${package} YARP_USE_${package} YARP_USE_SYSTEM_${package})

    #store all dependency flags for later export
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_USE_${package})

endmacro(checkbuildandset_dependency)

macro(print_dependency package)
    if(NOT YARP_USE_${package})
        message(STATUS "${package}: disabled")
    elseif(NOT YARP_USE_SYSTEM_${package})
        message(STATUS "${package}: compiling")
    elseif(${package}_FOUND)
        message(STATUS "${package}: found")
    else(NOT YARP_USE_${package})
        message(STATUS "${package}: not found")
    endif(NOT YARP_USE_${package})

    if (NOT YARP_HAS_${package} AND YARP_USE_${package})
        message("Warning: you requested to use the package ${package}, but it is unavailable (or was not found). This might lead to compile errors, we recommend you turn off the YARP_USE_${package} flag.")
    endif (NOT YARP_HAS_${package} AND YARP_USE_${package})
endmacro(print_dependency)



option(CREATE_YMANAGER "Do you want to compile Yarp module manager?" ON)
option(CREATE_GUIS "Do you want to compile GUIs" OFF)

if(CREATE_GUIS)
    option(CREATE_YARPVIEW "Do you want to compile yarpview?" ON)
    option(CREATE_GYARPMANAGER "Do you want to compile gyarpmanager?" ON)
    option(CREATE_YARPSCOPE "Do you want to create yarpscope?" OFF)
endif(CREATE_GUIS)

message(STATUS "Detecting required libraries")
message(STATUS "CMake modules directory: ${CMAKE_MODULE_PATH}")


find_package(SQLite QUIET)
checkbuildandset_dependency(SQLite)

if(CREATE_YMANAGER OR CREATE_YARPSCOPE)
    find_package(TinyXML QUIET)
    checkbuildandset_dependency(TinyXML)
endif(CREATE_YMANAGER OR CREATE_YARPSCOPE)

if(CREATE_GUIS)
    find_package(Gthread QUIET)
    checkandset_dependency(Gthread)

    if(YARP_USE_GTK2)
        if(CREATE_YARPSCOPE)
            find_package(GTK2 2.20 COMPONENTS gtk gtkmm QUIET)
        elseif(CREATE_GYARPMANAGER)
            find_package(GTK2 2.8 COMPONENTS gtk gtkmm QUIET)
        elseif(CREATE_YARPVIEW)
            find_package(GTK2 2.8 COMPONENTS gtk QUIET)
        endif(CREATE_YARPSCOPE)

        checkandset_dependency(GTK2)

    else(YARP_USE_GTK2)
        find_package(GtkPlus QUIET)
        checkandset_dependency(GtkPlus)
        if(CREATE_YARPSCOPE OR CREATE_GYARPMANAGER)
            find_package(GtkMM QUIET)
            checkandset_dependency(GtkMM)
        endif(CREATE_YARPSCOPE OR CREATE_GYARPMANAGER)
    endif(YARP_USE_GTK2)
endif(CREATE_GUIS)

if(CREATE_YARPSCOPE)
    find_package(GtkDatabox QUIET)
    checkbuildandset_dependency(GtkDatabox)
    find_package(GtkDataboxMM QUIET)
    checkbuildandset_dependency(GtkDataboxMM)
endif(CREATE_YARPSCOPE)

message(STATUS "I have found the following libraries:")

print_dependency(SQLite)
print_dependency(TinyXML)
print_dependency(Gthread)

if(YARP_USE_GTK2)
    print_dependency(GTK2)
else(YARP_USE_GTK2)
    print_dependency(GtkPlus)
    print_dependency(GtkMM)
endif(YARP_USE_GTK2)

print_dependency(GtkDatabox)
print_dependency(GtkDataboxMM)
