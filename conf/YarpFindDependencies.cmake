# Copyright (C) 2009  RobotCub Consortium
# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Lorenzo Natale, Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# This module checks if all the dependencies are installed and if the
# dependencies to build some parts of Yarp are satisfied.
# For every dependency, it creates the following variables:
#
# YARP_USE_${PACKAGE}: Can be disabled by the user if he doesn't want to use that
#                      dependency.
# YARP_HAS_${PACKAGE}: Internal flag. It should be used to check if a part of
#                      Yarp should be built. It is on if YARP_USE_${package} is
#                      on and either the package was found or will be built.
# YARP_USE_SYSTEM_${PACKAGE}: This flag is shown only for packages in the
#                             extern folder that were also found on the system
#                             (TRUE by default). If this flag is enabled, the
#                             system installed library will be used instead of
#                             the version shipped with Yarp.

# USEFUL MACROS:


# Check if a package is installed and set some cmake variables
macro(checkandset_dependency package)

    string(TOUPPER ${package} PKG)

    if(${package}_FOUND OR ${PKG}_FOUND)
        set(YARP_USE_${PKG} TRUE CACHE BOOL "Use package ${package}")
    else(${package}_FOUND OR ${PKG}_FOUND)
        set(YARP_USE_${PKG} FALSE CACHE BOOL "Use package ${package}")
    endif(${package}_FOUND OR ${PKG}_FOUND)

    if(YARP_USE_${PKG})
        if(${package}_FOUND OR ${PKG}_FOUND)
            set(YARP_HAS_${PKG} TRUE CACHE INTERNAL "Package ${package} found" FORCE)
        else(${package}_FOUND OR ${PKG}_FOUND)
            set(YARP_HAS_${PKG} FALSE CACHE INTERNAL "Package ${package} found" FORCE)
        endif(${package}_FOUND OR ${PKG}_FOUND)
    endif(YARP_USE_${PKG})

    unset(YARP_USE_SYSTEM_${PKG} CACHE)

    mark_as_advanced(YARP_USE_${package})

    #store all dependency flags for later export
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_USE_${PKG})
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_HAS_${PKG})
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_USE_SYSTEM_${PKG})

endmacro (checkandset_dependency)


# Check if a package is installed or if is going to be built and set some cmake variables
macro(checkbuildandset_dependency package)

    string(TOUPPER ${package} PKG)

    set(YARP_USE_${PKG} TRUE CACHE BOOL "Use package ${package}")
    mark_as_advanced(YARP_USE_${PKG})

    if (${package}_FOUND OR ${PKG}_FOUND)
        set(YARP_USE_SYSTEM_${PKG} TRUE CACHE BOOL "Use system-installed ${package}, rather than a private copy")
        mark_as_advanced(YARP_USE_SYSTEM_${PKG})
    else (${package}_FOUND OR ${PKG}_FOUND)
        # If package was not found we force it to be built
        unset(YARP_USE_SYSTEM_${PKG} CACHE)
    endif (${package}_FOUND OR ${PKG}_FOUND)

    if(YARP_USE_${PKG})
        set(YARP_HAS_${PKG} TRUE CACHE INTERNAL "Package ${package} found" FORCE)
    else(YARP_USE_${PKG})
        set(YARP_HAS_${PKG} FALSE CACHE INTERNAL "Package ${package} found" FORCE)
    endif(YARP_USE_${PKG})


    #store all dependency flags for later export
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_USE_${PKG})
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_HAS_${PKG})
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_USE_SYSTEM_${PKG})

endmacro(checkbuildandset_dependency)


# Check if a required package is installed.
macro(check_required_dependency package)

    string(TOUPPER ${package} PKG)

    if(NOT YARP_HAS_${PKG})
        message(FATAL_ERROR "Required package ${package} not found. Please install it to build yarp.")
#    else(NOT YARP_HAS_${PKG})
#        message(STATUS "${PKG} -> OK")
    endif(NOT YARP_HAS_${PKG})

endmacro(check_required_dependency)


# Check if an dependency required to enable an option is installed.
macro(check_optional_dependency optionname package)

    string(TOUPPER ${package} PKG)

    if(${optionname})
        if(NOT YARP_HAS_${PKG})
            message(FATAL_ERROR "Optional package ${package} not found. Please install it or disable the option \"${optionname}\" to build yarp.")
#        else(NOT YARP_HAS_${PKG})
#            message(STATUS "${PKG} ${optionname} -> OK")
        endif(NOT YARP_HAS_${PKG})
#    else(${optionname})
#        message(STATUS "${PKG} ${optionname} -> NOT REQUIRED")
    endif(${optionname})

endmacro(check_optional_dependency)


# Print status for a dependency
macro(print_dependency package)

    string(TOUPPER ${package} PKG)

    if(NOT YARP_USE_${PKG})
        message(STATUS "${package}: disabled")
    elseif(NOT YARP_USE_SYSTEM_${PKG})
        if(NOTYARP_HAS_${PKG})
            message(STATUS "${package}: compiling (not found)")
        else()
            message(STATUS "${package}: compiling (system package disabled)")
        endif()
    elseif(YARP_HAS_${PKG})
        message(STATUS "${package}: found")
    else(NOT YARP_USE_${PKG})
        message(STATUS "${package}: not found")
    endif(NOT YARP_USE_${PKG})
#    message(STATUS "YARP_USE_${PKG} = ${YARP_USE_${PKG}}")
#    message(STATUS "YARP_HAS_${PKG} = ${YARP_HAS_${PKG}}")
#    message(STATUS "YARP_USE_SYSTEM_${PKG} = ${YARP_USE_SYSTEM_${PKG}}")

endmacro(print_dependency)


# OPTIONS:

option(CREATE_LIB_MATH "Create math library libYARP_math?" OFF)
if(CREATE_LIB_MATH)
    # FIXME YARP_USE_ATLAS is probably not a good choice since it can make
    #       confusion with YARP_USE_Atlas (generated by checkandset_dependency
    #       macro)
    option(YARP_USE_ATLAS "Enable to link to Atlas for BLAS" OFF)
else(CREATE_LIB_MATH)
    unset(YARP_USE_ATLAS)
endif(CREATE_LIB_MATH)

option(CREATE_YMANAGER "Do you want to compile Yarp module manager?" ON)
option(CREATE_GUIS "Do you want to compile GUIs" OFF)

if(CREATE_GUIS)
    option(CREATE_YARPVIEW "Do you want to compile yarpview?" ON)
    option(CREATE_GYARPMANAGER "Do you want to compile gyarpmanager?" ON)
    option(CREATE_YARPSCOPE "Do you want to create yarpscope?" ON)
    option(CREATE_GYARPBUILDER "Do you want to compile Yarp application builder?" OFF)
else(CREATE_GUIS)
    unset(CREATE_YARPVIEW CACHE)
    unset(CREATE_GYARPMANAGER CACHE)
    unset(CREATE_YARPSCOPE CACHE)
    unset(CREATE_GYARPBUILDER CACHE)
endif(CREATE_GUIS)

message(STATUS "Detecting required libraries")
message(STATUS "CMake modules directory: ${CMAKE_MODULE_PATH}")


# FIND PACKAGES:

find_package(SQLite QUIET)
checkbuildandset_dependency(SQLite)

if(CREATE_LIB_MATH)
    find_package(GSL QUIET)
    checkandset_dependency(GSL)
    if(YARP_USE_ATLAS)
        find_package(Atlas QUIET)
        checkandset_dependency(Atlas)
    endif(YARP_USE_ATLAS)
endif(CREATE_LIB_MATH)

if(CREATE_YMANAGER OR CREATE_YARPSCOPE)
    find_package(TinyXML 2.6 QUIET)
    checkbuildandset_dependency(TinyXML)
endif(CREATE_YMANAGER OR CREATE_YARPSCOPE)

if(CREATE_GUIS)
    if(CREATE_YARPSCOPE)
        find_package(GTK2 2.20 COMPONENTS gtk gtkmm QUIET)
    elseif(CREATE_GYARPMANAGER)
        find_package(GTK2 2.8 COMPONENTS gtk gtkmm QUIET)
    elseif(CREATE_YARPVIEW)
        find_package(GTK2 2.8 COMPONENTS gtk QUIET)
    endif(CREATE_YARPSCOPE)

    checkandset_dependency(GTK2)

    ### FIXME: remove this check when stop supporting debian etch
    ### FIXME: FindGtkMM reports the wrong version on windows, since
    ###        this is a problem strictly related to debian etch, for
    ###        now we do this check only on UNIX.
    if (UNIX AND CREATE_YARPSCOPE)
        if (GtkMM_VERSION_MAJOR GREATER 2 OR GtkMM_VERSION_MAJOR EQUAL 2)
            if (GtkMM_VERSION_MINOR LESS 20)
                message(STATUS "Detected version of GtkMM that does not support yarpscope, turning off CREATE_YARPSCOPE")
                set(CREATE_YARPSCOPE FALSE CACHE BOOL "YARPSCOPE not supported" FORCE)
            endif()
        endif()
    endif(UNIX AND CREATE_YARPSCOPE)

endif(CREATE_GUIS)

if(CREATE_YARPSCOPE)
    find_package(GtkDatabox QUIET)
    checkbuildandset_dependency(GtkDatabox)
    find_package(GtkDataboxMM 0.9.3 QUIET)
    checkbuildandset_dependency(GtkDataboxMM)
endif(CREATE_YARPSCOPE)

if(CREATE_GYARPBUILDER)
    find_package(GooCanvas QUIET)
    checkbuildandset_dependency(GooCanvas)
    find_package(GooCanvasMM QUIET)
    checkbuildandset_dependency(GooCanvasMM)
endif(CREATE_GYARPBUILDER)


# CHECK DEPENDENCIES:

check_required_dependency(SQLite)
check_optional_dependency(CREATE_LIB_MATH GSL)
check_optional_dependency(YARP_USE_ATLAS Atlas)
check_optional_dependency(CREATE_YMANAGER TinyXML)
check_optional_dependency(CREATE_YARPSCOPE TinyXML)
check_optional_dependency(CREATE_GUIS GTK2)
check_optional_dependency(CREATE_YARPSCOPE GtkDatabox)
check_optional_dependency(CREATE_YARPSCOPE GtkDataboxMM)
check_optional_dependency(CREATE_GYARPBUILDER GooCanvas)
check_optional_dependency(CREATE_GYARPBUILDER GooCanvasMM)


# PRINT DEPENDENCIES STATUS:

message(STATUS "I have found the following libraries:")

print_dependency(SQLite)
print_dependency(GSL)
print_dependency(Atlas)
print_dependency(TinyXML)
print_dependency(GTK2)
print_dependency(GtkDatabox)
print_dependency(GtkDataboxMM)
print_dependency(GooCanvas)
print_dependency(GooCanvasMM)


#########################################################################
# Print information for user (CDash)
if (CREATE_LIB_MATH)
    message(STATUS "YARP_math selected for compilation")
endif()
if (CREATE_GUIS)
    message(STATUS "GUIs selected for compilation")
endif()
if (CREATE_YMANAGER)
    message(STATUS "yarp manager selected for compilation")
endif()
