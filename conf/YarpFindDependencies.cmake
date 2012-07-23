# Copyright (C) 2009  RobotCub Consortium
# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Lorenzo Natale, Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


macro(checkandset_dependency package)

    if (${package}_FOUND)
        set(YARP_HAS_${package} TRUE CACHE BOOL "Package ${package} found" FORCE)
        set(YARP_USE_${package} TRUE CACHE BOOL "Use package ${package}")
        message(STATUS "found ${package}")
    else (${package}_FOUND)
        set(YARP_HAS_${package} FALSE CACHE BOOL "" FORCE)
        set(YARP_USE_${package} FALSE CACHE BOOL "Use package ${package}")
    endif (${package}_FOUND)

    mark_as_advanced(YARP_HAS_${package} YARP_USE_${package})

    if (NOT ${package}_FOUND AND YARP_USE_${package})
        message("Warning: you requested to use the package ${package}, but it is unavailable (or was not found). This might lead to compile errors, we recommend you turn off the YARP_USE_${package} flag.")
    endif (NOT ${package}_FOUND AND YARP_USE_${package})

    #store all dependency flags for later export
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_USE_${package})

endmacro (checkandset_dependency)

macro(checkbuildandset_dependency package)

    if (${package}_FOUND)
        set(YARP_HAS_${package} TRUE CACHE BOOL "Package ${package} found" FORCE)
        set(YARP_USE_${package} TRUE CACHE BOOL "Use package ${package}")
        set(YARP_USE_SYSTEM_${package} TRUE CACHE BOOL "Use system-installed ${package}, rather than a private copy")
        message(STATUS "found ${package}")
    else (${package}_FOUND)
        set(YARP_HAS_${package} FALSE CACHE BOOL "" FORCE)
        set(YARP_USE_${package} FALSE CACHE BOOL "Use package ${package}")
        # If package was not found we force it to be built
        set(YARP_USE_SYSTEM_${package} FALSE CACHE BOOL "Use system-installed ${package}, rather than a private copy" FORCE)
    endif (${package}_FOUND)

    mark_as_advanced(YARP_HAS_${package} YARP_USE_${package}  YARP_USE_SYSTEM_${package})

    #store all dependency flags for later export
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_USE_${package})

endmacro (checkbuildandset_dependency)


message(STATUS "Detecting required libraries")
message(STATUS "CMake modules directory: ${CMAKE_MODULE_PATH}")


if(CREATE_GUIS)
    find_package(Gthread)
endif(CREATE_GUIS)

message(STATUS "I have found the following libraries:")

if(CREATE_GUIS)
    checkandset_dependency(Gthread)
endif(CREATE_GUIS)
