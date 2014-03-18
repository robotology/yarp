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

    # YARP_HAS_SYSTEM_${PKG}
    if(${package}_FOUND OR ${PKG}_FOUND)
        set(YARP_HAS_SYSTEM_${PKG} TRUE)
    else()
        set(YARP_HAS_SYSTEM_${PKG} FALSE)
    endif()

    # YARP_USE_${PKG}
    cmake_dependent_option(YARP_USE_${PKG} "Use package ${package}" TRUE
                           YARP_HAS_SYSTEM_${PKG} FALSE)
    mark_as_advanced(YARP_USE_${package})

    # YARP_USE_SYSTEM_${PKG}
    set(YARP_USE_SYSTEM_${PKG} ${YARP_USE_${PKG}} CACHE INTERNAL "Use system-installed ${package}, rather than a private copy (recommended)" FORCE)

    # YARP_HAS_${PKG}
    if(${YARP_HAS_SYSTEM_${PKG}})
        set(YARP_HAS_${PKG} ${YARP_USE_${PKG}})
    endif()

    #store all dependency flags for later export
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_HAS_${PKG})
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_USE_${PKG})
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_HAS_SYSTEM_${PKG})
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_USE_SYSTEM_${PKG})

endmacro (checkandset_dependency)


# Check if a package is installed or if is going to be built and set some cmake variables
macro(checkbuildandset_dependency package)

    string(TOUPPER ${package} PKG)

    # YARP_HAS_SYSTEM_${PKG}
    if (${package}_FOUND OR ${PKG}_FOUND)
        set(YARP_HAS_SYSTEM_${PKG} TRUE)
    else()
        set(YARP_HAS_SYSTEM_${PKG} FALSE)
    endif()

    # YARP_USE_${PKG}
    option(YARP_USE_${PKG} "Use package ${package}" TRUE)
    mark_as_advanced(YARP_USE_${PKG})

    # YARP_USE_SYSTEM_${PKG}
    cmake_dependent_option(YARP_USE_SYSTEM_${PKG} "Use system-installed ${package}, rather than a private copy (recommended)" TRUE
                           "YARP_HAS_SYSTEM_${PKG};YARP_USE_${PKG}" FALSE)
    mark_as_advanced(YARP_USE_SYSTEM_${PKG})

    # YARP_HAS_${PKG}
    set(YARP_HAS_${PKG} ${YARP_USE_${PKG}})

    #store all dependency flags for later export
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_HAS_${PKG})
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_USE_${PKG})
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_HAS_SYSTEM_${PKG})
    set_property(GLOBAL APPEND PROPERTY YARP_DEPENDENCIES_FLAGS YARP_USE_SYSTEM_${PKG})

endmacro(checkbuildandset_dependency)


# Check if a required package is installed.
macro(check_required_dependency package)

    string(TOUPPER ${package} PKG)

    if(NOT YARP_HAS_${PKG})
        message(FATAL_ERROR "Required package ${package} not found. Please install it to build yarp.")
#    else()
#        message(STATUS "${PKG} -> OK")
    endif()

endmacro(check_required_dependency)


# Check if a dependency required to enable an option is installed.
macro(check_optional_dependency optionname package)

    string(TOUPPER ${package} PKG)

    if(${optionname})
        if(NOT YARP_HAS_${PKG})
            message(FATAL_ERROR "Optional package ${package} not found. Please install it or disable the option \"${optionname}\" to build yarp.")
#        else()
#            message(STATUS "${PKG} ${optionname} -> OK")
        endif()
#    else()
#        message(STATUS "${PKG} ${optionname} -> NOT REQUIRED")
    endif()

endmacro(check_optional_dependency)


# Check if a dependency required to disable an option is installed.
macro(check_skip_dependency optionname package)
    string(TOUPPER ${package} PKG)

    if(NOT ${optionname})
        if(NOT YARP_HAS_${PKG})
            message(FATAL_ERROR "Optional package ${package} not found. Please install it or enable the option \"${optionname}\" to build yarp.")
        endif()
    endif()
endmacro()


# Print status for a dependency
macro(print_dependency package)

    string(TOUPPER ${package} PKG)

#    message("YARP_HAS_SYSTEM_${PKG} = ${YARP_HAS_SYSTEM_${PKG}}")
#    message("YARP_USE_${PKG} = ${YARP_USE_${PKG}}")
#    message("YARP_USE_SYSTEM_${PKG} = ${YARP_USE_SYSTEM_${PKG}}")
#    message("YARP_HAS_${PKG} = ${YARP_HAS_${PKG}}")
    if(NOT YARP_USE_${PKG})
        message(STATUS " +++ ${package}: disabled")
    elseif(NOT YARP_HAS_${PKG})
        message(STATUS " +++ ${package}: not found")
    elseif(YARP_HAS_SYSTEM_${PKG} AND YARP_USE_SYSTEM_${PKG})
        message(STATUS " +++ ${package}: found")
    elseif(YARP_HAS_SYSTEM_${PKG})
        message(STATUS " +++ ${package}: compiling (system package disabled)")
    else()
        message(STATUS " +++ ${package}: compiling (not found)")
    endif()

endmacro(print_dependency)


# OPTIONS:

option(SKIP_ACE "Compile YARP without ACE (Linux only, TCP only, limited functionality)" OFF)
mark_as_advanced(SKIP_ACE)


option(CREATE_LIB_MATH "Create math library libYARP_math?" OFF)
if(CREATE_LIB_MATH)
    # FIXME YARP_USE_ATLAS is probably not a good choice since it can make
    #       confusion with YARP_USE_Atlas (generated by checkandset_dependency
    #       macro)
    option(YARP_USE_ATLAS "Enable to link to Atlas for BLAS" OFF)
else()
    unset(YARP_USE_ATLAS)
endif()

option(CREATE_YMANAGER "Do you want to compile Yarp module manager?" ON)
option(CREATE_GUIS "Do you want to compile GUIs" OFF)

if(CREATE_GUIS)
    option(CREATE_YARPVIEW "Do you want to compile yarpview?" ON)
    option(CREATE_GYARPMANAGER "Do you want to compile gyarpmanager?" ON)
    option(CREATE_YARPSCOPE "Do you want to create yarpscope?" ON)
    option(CREATE_GYARPBUILDER "Do you want to compile Yarp application builder?" OFF)
else()
    unset(CREATE_YARPVIEW CACHE)
    unset(CREATE_GYARPMANAGER CACHE)
    unset(CREATE_YARPSCOPE CACHE)
    unset(CREATE_GYARPBUILDER CACHE)
endif()

message(STATUS "Detecting required libraries")
message(STATUS "CMake modules directory: ${CMAKE_MODULE_PATH}")


# FIND PACKAGES:

if(SKIP_ACE)
    set(ACE_LIBRARIES pthread rt)
else()
    find_package(ACE QUIET)
    checkandset_dependency(ACE)
    # FIXME Replace SKIP_ACE with YARP_USE_ACE
    unset(YARP_USE_ACE CACHE)

    # __ACE_INLINE__ is needed in some configurations
    if(NOT ACE_COMPILES_WITHOUT_INLINE_RELEASE)
        foreach(_config ${YARP_OPTIMIZED_CONFIGURATIONS})
            string(TOUPPER ${_config} _CONFIG)
            set(CMAKE_C_FLAGS_${_CONFIG} "${CMAKE_C_FLAGS_${_CONFIG}} -D__ACE_INLINE__")
            set(CMAKE_CXX_FLAGS_${_CONFIG} "${CMAKE_CXX_FLAGS_${_CONFIG}} -D__ACE_INLINE__")
        endforeach()
    endif()

    if(NOT ACE_COMPILES_WITHOUT_INLINE_DEBUG)
        foreach(_config ${YARP_DEBUG_CONFIGURATIONS})
            string(TOUPPER ${_config} _CONFIG)
            set(CMAKE_C_FLAGS_${_CONFIG} "${CMAKE_C_FLAGS_${_CONFIG}} -D__ACE_INLINE__")
            set(CMAKE_CXX_FLAGS_${_CONFIG} "${CMAKE_CXX_FLAGS_${_CONFIG}} -D__ACE_INLINE__")
        endforeach()
    endif()
endif()

find_package(SQLite QUIET)
checkbuildandset_dependency(SQLite)

find_package(Readline)
checkandset_dependency(Readline)

if(CREATE_LIB_MATH)
    find_package(GSL QUIET)
    checkandset_dependency(GSL)
    if(YARP_USE_ATLAS)
        find_package(Atlas QUIET)
        checkandset_dependency(Atlas)
    endif()
endif()

if(CREATE_YMANAGER OR CREATE_YARPSCOPE)
    find_package(TinyXML 2.6 QUIET)
    checkbuildandset_dependency(TinyXML)
endif()

if(CREATE_GUIS)
    if(CREATE_YARPSCOPE)
        find_package(GTK2 2.20 COMPONENTS gtk gtkmm QUIET)
    elseif(CREATE_GYARPMANAGER)
        find_package(GTK2 2.8 COMPONENTS gtk gtkmm QUIET)
    elseif(CREATE_YARPVIEW)
        find_package(GTK2 2.8 COMPONENTS gtk QUIET)
    endif()

    checkandset_dependency(GTK2)

    ### FIXME: remove this check when stop supporting debian etch
    ### FIXME: FindGtkMM reports the wrong version on windows, since
    ###        this is a problem strictly related to debian etch, for
    ###        now we do this check only on UNIX.
    if (UNIX AND CREATE_YARPSCOPE)
        if (GtkMM_VERSION_MAJOR GREATER 2 OR GtkMM_VERSION_MAJOR EQUAL 2)
            if (GtkMM_VERSION_MINOR LESS 20)
                message(STATUS "Detected version of GtkMM that does not support yarpscope, turning off CREATE_YARPSCOPE")
                set_property(CACHE CREATE_YARPSCOPE PROPERTY VALUE FALSE)
            endif()
        endif()
    endif()

endif()

if(CREATE_YARPSCOPE)
    find_package(GtkDatabox QUIET)
    checkbuildandset_dependency(GtkDatabox)
    find_package(GtkDataboxMM 0.9.3 QUIET)
    checkbuildandset_dependency(GtkDataboxMM)
endif()

if(CREATE_GYARPBUILDER)
    find_package(GooCanvas QUIET)
    checkbuildandset_dependency(GooCanvas)
    find_package(GooCanvasMM QUIET)
    checkbuildandset_dependency(GooCanvasMM)
endif()


if(ENABLE_yarpmod_opencv_grabber)
    find_package(OpenCV QUIET)
    checkandset_dependency(OpenCV)
endif()



# CHECK DEPENDENCIES:

check_skip_dependency(SKIP_ACE ACE)
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
check_optional_dependency(ENABLE_yarpmod_opencv_grabber OpenCV)


# PRINT DEPENDENCIES STATUS:

message(STATUS "I have found the following libraries:")

print_dependency(ACE)
print_dependency(SQLite)
print_dependency(GSL)
print_dependency(Atlas)
print_dependency(TinyXML)
print_dependency(GTK2)
print_dependency(GtkDatabox)
print_dependency(GtkDataboxMM)
print_dependency(GooCanvas)
print_dependency(GooCanvasMM)
print_dependency(Readline)
print_dependency(OpenCV)


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
