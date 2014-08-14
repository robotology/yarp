# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

#########################################################################
##
## This file provides a set of macros for building bundles of plugins.
## Sample use:
##
##  YARP_BEGIN_PLUGIN_LIBRARY(libname)
##    ADD_SUBDIRECTORY(plugin1)
##    ADD_SUBDIRECTORY(plugin2)
##    ...
##  YARP_END_PLUGIN_LIBRARY(libname)
##  YARP_ADD_PLUGIN_LIBRARY_EXECUTABLE(libnamedev libname)
##
## This sample would create two CMake targets, "libname" (a library)
## and libnamedev (an executable).  It also defines a list:
##   ${libname_LIBRARIES}
## which contains a list of all library targets created within
## the plugin directories plugin1, plugin2, ...
##
## The "libname" library links with every library in the subdirectories
## (which can be made individually optional using CMake options),
## and provides a method to initialize them all.
##
## The executable is a test program that links and initializes
## the "libname" library, making the plugins accessible.
##
## Plugins come in two types, carriers and devices.
## To let YARP know how to initialize them, add lines like
## this in the CMakeLists.txt files the plugin subdirectories:
##
##   YARP_PREPARE_DEVICE(microphone TYPE MicrophoneDeviceDriver
##                  INCLUDE MicrophoneDeviceDriver.h WRAPPER grabber)
## (the WRAPPER is optional), or:
##   YARP_PREPARE_CARRIER(new_carrier TYPE TheCarrier INCLUDE ...)
##
#########################################################################

include(GNUInstallDirs)

## Skip this whole file if it has already been included
if(NOT COMMAND YARP_END_PLUGIN_LIBRARY)

get_property(YARP_TREE_BUILD GLOBAL PROPERTY YARP_TREE_BUILD)
if(YARP_TREE_BUILD)
    # When compiling YARP,
    # Prepare path information for the benefit of clients

    get_target_property(YARP_LIBRARY_PATH YARP_OS LOCATION)
    get_target_property(YARP_LIBRARY_TYPE YARP_OS TYPE)
    string(REPLACE "STATIC_LIBRARY" "static" YARP_LIBRARY_TYPE ${YARP_LIBRARY_TYPE})
    string(REPLACE "SHARED_LIBRARY" "shared" YARP_LIBRARY_TYPE ${YARP_LIBRARY_TYPE})
    get_filename_component(YARP_LIBRARY_EXTENSION ${YARP_LIBRARY_PATH} EXT)
    get_filename_component(YARP_LIBRARY_PREFIX ${YARP_LIBRARY_PATH} NAME_WE)
    string(REPLACE "YARP_OS" "" YARP_LIBRARY_PREFIX ${YARP_LIBRARY_PREFIX})
    get_filename_component(YARP_LIBRARY_PATH ${YARP_LIBRARY_PATH} ABSOLUTE)
    get_filename_component(YARP_LIBRARY_PATH ${YARP_LIBRARY_PATH} PATH)
    configure_file(${YARP_MODULE_DIR}/template/YarpPluginPath.cmake
        ${CMAKE_BINARY_DIR}/plugins/path.ini @ONLY)
    set(YARP_LIBRARY_PATH ${CMAKE_INSTALL_PREFIX}/lib)
    configure_file(${YARP_MODULE_DIR}/template/YarpPluginPath.cmake
        ${CMAKE_BINARY_DIR}/path_for_install.ini @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/path_for_install.ini RENAME path.ini COMPONENT configuration DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/yarp/plugins)
endif(YARP_TREE_BUILD)

#########################################################################
# YARP_BEGIN_PLUGIN_LIBRARY: this macro makes sure that all the hooks
# needed for creating a plugin library are in place.  Between
# this call, and a subsequent call to END_PLUGIN_LIBRARY, the
# X_YARP_PLUGIN_MODE variable is set.  While this mode is set,
# any library targets created are tracked in a global list.
# Calls to this macro may be nested.
#
macro(YARP_BEGIN_PLUGIN_LIBRARY bundle_name)

    if(X_YARP_PLUGIN_MODE)

        # If we are nested inside a larger plugin block, we don't
        # have to do anything.
        message(STATUS "nested library ${bundle_name}")

    else(X_YARP_PLUGIN_MODE)

        # If we are the outermost plugin block, then we need to set up
        # everything for tracking the plugins within that block.

        # Make a record of the fact that we are now within a plugin
        set(X_YARP_PLUGIN_MODE TRUE)

        # Declare that we are starting to compile the given plugin library
        message(STATUS "starting plugin library: ${bundle_name}")

        # Prepare a directory for automatically generated boilerplate code.
        set(X_YARP_PLUGIN_GEN ${CMAKE_BINARY_DIR}/generated_code)
        if(NOT EXISTS ${X_YARP_PLUGIN_GEN})
            file(MAKE_DIRECTORY ${X_YARP_PLUGIN_GEN})
        endif(NOT EXISTS ${X_YARP_PLUGIN_GEN})

        # Choose a prefix for CMake options related to this library
        set(X_YARP_PLUGIN_PREFIX "${bundle_name}_")

        # Set a flag to let individual modules know that they are being
        # compiled as part of a bundle, and not standalone.  Developers
        # use this flag to inhibit compilation of test programs and
        # the like.
        set(COMPILE_PLUGIN_LIBRARY TRUE)
        set(COMPILE_DEVICE_LIBRARY TRUE) # an old name for the flag

        # Record the name of the plugin library name
        set(X_YARP_PLUGIN_MASTER ${bundle_name})

        # Set some properties to an empty state
        set_property(GLOBAL PROPERTY YARP_BUNDLE_PLUGINS) # list of plugins
        set_property(GLOBAL PROPERTY YARP_BUNDLE_STUBS)   # list of stubs
        set_property(GLOBAL PROPERTY YARP_BUNDLE_STUB_CODE) # initializers
        set_property(GLOBAL PROPERTY YARP_BUNDLE_OWNERS)   # owner library
        set_property(GLOBAL PROPERTY YARP_BUNDLE_LIBS)    # list of library targets
        set_property(GLOBAL PROPERTY YARP_BUNDLE_LINKS)   # list of link directories
        set_property(GLOBAL PROPERTY YARP_BUNDLE_CODE)    # list of generated code

        # One glitch is that if plugins are used within YARP, rather
        # than in an external library, then "find_package(YARP)" will
        # not work correctly yet.  We simulate the operation of
        # find_package(YARP) here if needed, using properties
        # maintained during the YARP build.
        get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
        if(YARP_TREE_INCLUDE_DIRS)
            # Simulate the operation of find_package(YARP)
            set(YARP_FOUND TRUE)
            get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
            get_property(YARP_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
            get_property(YARP_DEFINES GLOBAL PROPERTY YARP_DEFS)
        else(YARP_TREE_INCLUDE_DIRS)
            find_package(YARP REQUIRED)
        endif(YARP_TREE_INCLUDE_DIRS)

    endif(X_YARP_PLUGIN_MODE)

endmacro(YARP_BEGIN_PLUGIN_LIBRARY)


#########################################################################
# YARP_ADD_PLUGIN_NORMALIZED macro is an internal command to convert a
# plugin declaration to code, and to set up CMake flags for controlling
# compilation of that device.  This macro is called by YARP_PREPARE_PLUGIN
# which is the user-facing macro.  YARP_PREPARE_PLUGIN parses
# a flexible set of arguments, then passes them to YARP_ADD_PLUGIN_NORMALIZED
# in a clean canonical order.
#
macro(YARP_ADD_PLUGIN_NORMALIZED plugin_name type include wrapper category libname)

    # Append the current source directory to the set of include paths.
    # Developers seem to expect #include "foo.h" to work if foo.h is
    # in their module directory.
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})

    # Figure out a decent filename for the code we are about to
    # generate.  If all else fails, the code will get dumped in
    # the current binary directory.
    set(fdir ${X_YARP_PLUGIN_GEN})
    if(NOT fdir)
        set(fdir ${CMAKE_CURRENT_BINARY_DIR})
    endif(NOT fdir)

    # We'll be expanding the code in template/yarp_plugin_*.cpp.in using
    # the following variables:

    set(YARPDEV_NAME "${plugin_name}")
    set(YARPDEV_TYPE "${type}")
    set(YARPDEV_INCLUDE "${include}")
    set(YARPDEV_WRAPPER "${wrapper}")
    set(YARPDEV_CATEGORY "${category}")
    set(YARPDEV_LIBNAME "${libname}")

    #set(ENABLE_YARPDEV_NAME "1")

    # Set up a flag to enable/disable compilation of this plugin.
    set(X_MYNAME "${X_YARP_PLUGIN_PREFIX}${plugin_name}")
    if(NOT COMPILE_BY_DEFAULT)
        set(COMPILE_BY_DEFAULT FALSE)
    endif(NOT COMPILE_BY_DEFAULT)
    set(ENABLE_${X_MYNAME} ${COMPILE_BY_DEFAULT} CACHE BOOL "Enable/disable compilation of ${X_MYNAME}")

    # Set some convenience variables based on whether the plugin
    # is enabled or disabled.
    set(ENABLE_${plugin_name} ${ENABLE_${X_MYNAME}})
    if(ENABLE_${plugin_name})
        set(SKIP_${plugin_name} FALSE)
        set(SKIP_${X_MYNAME} FALSE)
    else(ENABLE_${plugin_name})
        set(SKIP_${plugin_name} TRUE)
        set(SKIP_${X_MYNAME} TRUE)
    endif(ENABLE_${plugin_name})

    # If the plugin is enabled, add the appropriate source code into
    # the library source list.
    if(ENABLE_${X_MYNAME})
        # Go ahead and prepare some code to wrap this plugin.
        set(fname ${fdir}/yarpdev_add_${plugin_name}.cpp)
        set(fname_stub ${fdir}/yarpdev_stub_${plugin_name}.cpp)
        configure_file(${YARP_MODULE_DIR}/template/yarp_plugin_${category}.cpp.in
                       ${fname} @ONLY)

        if(YARPDEV_CATEGORY STREQUAL "device")
            # if the plugin is a device, use the template to generate the .ini manifest file
            if(YARPDEV_WRAPPER STREQUAL "" )
                set(PRINT_WRAPPER "")
            else(YARPDEV_WRAPPER STREQUAL "")
                set(PRINT_WRAPPER "wrapper")
            endif(YARPDEV_WRAPPER STREQUAL "")

            # if the libname param is not explicitly provided, use the plugin name as name
            # of the library file too (i.e.:  ${plugin_name}.so, ${plugin_name}.dll)
            if(YARPDEV_LIBNAME STREQUAL "" )
                set(YARPDEV_LIBNAME ${plugin_name} )
            endif(YARPDEV_LIBNAME STREQUAL "")

            configure_file(${YARP_MODULE_DIR}/template/YarpPluginConfFile.cmake.in
                           ${CMAKE_CURRENT_SOURCE_DIR}/${plugin_name}_manifest.ini @ONLY)

            # message(STATUS " generating file ${CMAKE_CURRENT_SOURCE_DIR}/${plugin_name}.ini")
        endif(YARPDEV_CATEGORY STREQUAL "device")

        set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_PLUGINS ${plugin_name})
        set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_CODE ${fname})
        set(YARP_PLUGIN_ACTIVE TRUE)
        message(STATUS " +++ plugin ${plugin_name}, ENABLE_${plugin_name} is set")
    else(ENABLE_${X_MYNAME})
        message(STATUS " +++ plugin ${plugin_name}, SKIP_${plugin_name} is set")
    endif(ENABLE_${X_MYNAME})

    # We are done!

endmacro(YARP_ADD_PLUGIN_NORMALIZED)


#########################################################################
# YARP_PREPARE_PLUGIN macro lets a developer declare a plugin using a
# statement like:
#    YARP_PREPARE_PLUGIN(foo CATEGORY device TYPE FooDriver INCLUDE FooDriver.h)
# or
#    YARP_PREPARE_PLUGIN(moto CATEGORY device TYPE Moto INCLUDE moto.h WRAPPER controlboard)
# This macro is just a simple parser and calls YARP_ADD_PLUGIN_NORMALIZED to
# do the actual work.
#
macro(YARP_PREPARE_PLUGIN plugin_name)
    set(X_EXPECT_TYPE FALSE)
    set(X_EXPECT_INCLUDE FALSE)
    set(X_EXPECT_LIBNAME FALSE)
    set(X_THE_TYPE "")
    set(X_THE_INCLUDE "")
    set(X_THE_WRAPPER "")
    set(X_THE_LIBNAME "")

    foreach(arg ${ARGN})
        if(X_EXPECT_TYPE)
            set(X_THE_TYPE ${arg})
            set(X_EXPECT_TYPE FALSE)
        endif(X_EXPECT_TYPE)
        if(X_EXPECT_INCLUDE)
            set(X_THE_INCLUDE ${arg})
            set(X_EXPECT_INCLUDE FALSE)
        endif(X_EXPECT_INCLUDE)
        if(EXPECT_WRAPPER)
            set(X_THE_WRAPPER ${arg})
            set(EXPECT_WRAPPER FALSE)
        endif(EXPECT_WRAPPER)
        if(EXPECT_CATEGORY)
            set(THE_CATEGORY ${arg})
            set(EXPECT_CATEGORY FALSE)
        endif(EXPECT_CATEGORY)
        if(X_EXPECT_LIBNAME)
            set(X_THE_LIBNAME ${arg})
            message(INFO " libname found as ${X_THE_LIBNAME}")
            set(X_EXPECT_LIBNAME FALSE)
        endif(X_EXPECT_LIBNAME)

        if(${arg} STREQUAL "LIBNAME")  #it does not work without the ${}!!
            set(X_EXPECT_LIBNAME TRUE)
        message(STATUS " found LIBNAME" )
        endif(${arg} STREQUAL "LIBNAME")

        if(arg STREQUAL "TYPE")
            set(X_EXPECT_TYPE TRUE)
        endif(arg STREQUAL "TYPE")
        if(arg STREQUAL "INCLUDE")
            set(X_EXPECT_INCLUDE TRUE)
        endif(arg STREQUAL "INCLUDE")
        if(arg STREQUAL "WRAPPER")
            set(EXPECT_WRAPPER TRUE)
        endif(arg STREQUAL "WRAPPER")
        if(arg STREQUAL "CATEGORY")
            set(EXPECT_CATEGORY TRUE)
        endif(arg STREQUAL "CATEGORY")


    endforeach(arg)
    if(X_THE_TYPE AND X_THE_INCLUDE)
        yarp_add_plugin_normalized(${plugin_name} ${X_THE_TYPE} ${X_THE_INCLUDE} "${X_THE_WRAPPER}" "${THE_CATEGORY}" "${X_THE_LIBNAME}")
    else(X_THE_TYPE AND X_THE_INCLUDE)
        message(STATUS "Not enough information to create ${plugin_name}")
        message(STATUS "  type:    ${X_THE_TYPE}")
        message(STATUS "  include: ${X_THE_INCLUDE}")
        message(STATUS "  wrapper: ${X_THE_WRAPPER}")
        message(STATUS "  category: ${THE_CATEGORY}")
    endif(X_THE_TYPE AND X_THE_INCLUDE)
endmacro(YARP_PREPARE_PLUGIN plugin_name)



#########################################################################
# YARP_PREPARE_DEVICE macro lets a developer declare a device plugin using a
# statement like:
#    YARP_PREPARE_PLUGIN(moto CATEGORY device TYPE Moto INCLUDE moto.h WRAPPER controlboard)
#
macro(YARP_PREPARE_DEVICE)
    yarp_prepare_plugin(${ARGN} CATEGORY device)
endmacro(YARP_PREPARE_DEVICE)



#########################################################################
# YARP_PREPARE_CARRIER macro lets a developer declare a carrier plugin using a
# statement like:
#    YARP_PREPARE_CARRIER(foo TYPE FooCarrier INCLUDE FooCarrier.h)
#
macro(YARP_PREPARE_CARRIER)
    yarp_prepare_plugin(${ARGN} CATEGORY carrier)
endmacro(YARP_PREPARE_CARRIER)



#########################################################################
# YARP_ADD_PLUGIN macro tracks plugin libraries.  We want to
# be later able to link against them all as a group.
#
macro(YARP_ADD_PLUGIN LIBNAME)
    # we check to see if the ADD_LIBRARY call is an import, and ignore
    # if so - we don't need to do anything about imports.
    set(X_IS_IMPORTED FALSE)
    foreach(arg ${ARGN})
        if("${arg}" STREQUAL "IMPORTED")
            set(X_IS_IMPORTED TRUE)
        endif("${arg}" STREQUAL "IMPORTED")
    endforeach(arg)
    if(NOT X_IS_IMPORTED)
        # The user is adding a bone-fide plugin library.  We add it,
        # while inserting any generated source code needed for initialization.
        get_property(srcs GLOBAL PROPERTY YARP_BUNDLE_CODE)
        foreach(s ${srcs})
            set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_OWNERS ${LIBNAME})
        endforeach(s)
        add_library(${LIBNAME} ${srcs} ${ARGN})
        # Add the library to the list of plugin libraries.
        set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_LIBS ${LIBNAME})
        # Reset the list of generated source code to empty.
        set_property(GLOBAL PROPERTY YARP_BUNDLE_CODE)
        if(YARP_TREE_INCLUDE_DIRS)
            # If compiling YARP, we go ahead and set up installing this
            # target.  It isn't safe to do this outside of YARP though.
            install(TARGETS ${LIBNAME}
                    EXPORT YARP
                    COMPONENT runtime
                    DESTINATION ${CMAKE_INSTALL_LIBDIR})
        endif(YARP_TREE_INCLUDE_DIRS)
    endif(NOT X_IS_IMPORTED)
endmacro(YARP_ADD_PLUGIN)



#########################################################################
# Lightly redefine LINK_DIRECTORIES to track link path changes.  Rarely
# needed.
#
macro(LINK_DIRECTORIES)
    _LINK_DIRECTORIES(${ARGN})
    if(X_YARP_PLUGIN_MODE)
        # Add to the list of linked directories.
        set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_LINKS ${ARGN})
        set_property(GLOBAL APPEND PROPERTY YARP_TREE_LINK_DIRS ${ARGN})
    endif(X_YARP_PLUGIN_MODE)
endmacro(LINK_DIRECTORIES)



#########################################################################
# Lightly redefine FIND_PACKAGE to skip calls to FIND_PACKAGE(YARP).
# YARP dependencies are guaranteed to have already been satisfied.
# And if we are compiling YARP, the use of FIND_PACKAGE(YARP) will lead
# to problems.
#
macro(FIND_PACKAGE LIBNAME)
    if(NOT X_YARP_PLUGIN_MODE)
        # pass on call without looking at it
        _FIND_PACKAGE(${LIBNAME} ${ARGN})
    endif(NOT X_YARP_PLUGIN_MODE)
    if(NOT "${LIBNAME}" STREQUAL "YARP")
        # Skipping requests for YARP, we already have it
        _FIND_PACKAGE(${LIBNAME} ${ARGN})
    endif(NOT "${LIBNAME}" STREQUAL "YARP")
endmacro(FIND_PACKAGE LIBNAME)



#########################################################################
# YARP_END_PLUGIN_LIBRARY macro finalizes a plugin library if this is
# the outermost plugin library block, otherwise it propagates
# all collected information to the plugin library block that wraps
# it.
#
macro(YARP_END_PLUGIN_LIBRARY bundle_name)
    message(STATUS "ending plugin library: ${bundle_name}")
    # make sure we are the outermost plugin library, if nesting is present.
    if("${bundle_name}" STREQUAL "${X_YARP_PLUGIN_MASTER}")
        # generate code to call all plugin initializers
        set(YARP_LIB_NAME ${X_YARP_PLUGIN_MASTER})
        get_property(devs GLOBAL PROPERTY YARP_BUNDLE_PLUGINS)
        get_property(owners GLOBAL PROPERTY YARP_BUNDLE_OWNERS)
        set(YARP_CODE_PRE)
        set(YARP_CODE_POST)
        foreach(dev ${devs})
            if(NOT owners)
                message(SEND_ERROR "Empty list (owner), this is likely due to a previous error, check the output of CMake above.")
            endif(NOT owners)
            list(GET owners 0 owner)
            list(REMOVE_AT owners 0)
            set(YARP_CODE_PRE "${YARP_CODE_PRE}\nextern YARP_PLUGIN_IMPORT void add_owned_${dev}(const char *str);")
            set(YARP_CODE_POST "${YARP_CODE_POST}\n    add_owned_${dev}(\"${owner}\");")
        endforeach(dev)
        configure_file(${YARP_MODULE_DIR}/template/yarpdev_lib.cpp.in
                       ${X_YARP_PLUGIN_GEN}/add_${X_YARP_PLUGIN_MASTER}_plugins.cpp @ONLY)
        configure_file(${YARP_MODULE_DIR}/template/yarpdev_lib.h.in
                       ${X_YARP_PLUGIN_GEN}/add_${X_YARP_PLUGIN_MASTER}_plugins.h @ONLY)
        get_property(code GLOBAL PROPERTY YARP_BUNDLE_CODE)
        get_property(code_stub GLOBAL PROPERTY YARP_BUNDLE_STUB_CODE)
        include_directories(${YARP_INCLUDE_DIRS})
        get_property(libs GLOBAL PROPERTY YARP_BUNDLE_LIBS)
        get_property(links GLOBAL PROPERTY YARP_BUNDLE_LINKS)
        if(links)
            _link_directories(${links})
        endif(links)
        # add the library initializer code
        add_library(${X_YARP_PLUGIN_MASTER} ${code} ${code_stub} ${X_YARP_PLUGIN_GEN}/add_${X_YARP_PLUGIN_MASTER}_plugins.cpp)

        if(TARGET YARP_OS)
            # Building YARP
            target_link_libraries(${X_YARP_PLUGIN_MASTER} LINK_PRIVATE YARP_OS)
        else()
            target_link_libraries(${X_YARP_PLUGIN_MASTER} LINK_PRIVATE YARP::YARP_OS)
        endif()
        target_link_libraries(${X_YARP_PLUGIN_MASTER} LINK_PRIVATE ${libs})
        # give user access to a list of all the plugin libraries
        set(${X_YARP_PLUGIN_MASTER}_LIBRARIES ${libs})
        set(X_YARP_PLUGIN_MODE FALSE) # neutralize redefined methods
    endif("${bundle_name}" STREQUAL "${X_YARP_PLUGIN_MASTER}")
endmacro(YARP_END_PLUGIN_LIBRARY bundle_name)



#########################################################################
# YARP_ADD_PLUGIN_LIBRARY_EXECUTABLE macro expands a simple test program
# for a named device library.
#
macro(YARP_ADD_PLUGIN_LIBRARY_EXECUTABLE exename bundle_name)
    configure_file(${YARP_MODULE_DIR}/template/yarpdev_lib_main.cpp.in
                   ${X_YARP_PLUGIN_GEN}/yarpdev_${bundle_name}.cpp @ONLY)
    add_executable(${exename} ${X_YARP_PLUGIN_GEN}/yarpdev_${bundle_name}.cpp)
    target_link_libraries(${exename} ${bundle_name})
endmacro(YARP_ADD_PLUGIN_LIBRARY_EXECUTABLE)



#########################################################################
# YARP_ADD_CARRIER_FINGERPRINT macro gives YARP a config file that will help
# detect that a message is in a particular protocol, even if support for
# that protocol has not yet loaded:
#    YARP_ADD_CARRIER_FINGERPRINT(carrier.ini carrier1 carrier2)
#
macro(YARP_ADD_CARRIER_FINGERPRINT file_name)
    get_filename_component(out_name ${file_name} NAME)
    configure_file(${file_name} ${CMAKE_BINARY_DIR}/plugins/${out_name} COPYONLY)
    if(YARP_TREE_INCLUDE_DIRS)
        install(FILES ${CMAKE_BINARY_DIR}/plugins/${out_name}
                COMPONENT runtime
                DESTINATION share/yarp/plugins)
    endif(YARP_TREE_INCLUDE_DIRS)
endmacro(YARP_ADD_CARRIER_FINGERPRINT)


#########################################################################
# YARP_ADD_DEVICE_FINGERPRINT macro gives YARP a config file that will help
# give information about a device that has not yet loaded
#    YARP_ADD_DEVICE_FINGERPRINT(device.ini device1 device2)
#
macro(YARP_ADD_DEVICE_FINGERPRINT)
    # no difference between fingerprint macros
    YARP_ADD_CARRIER_FINGERPRINT(${ARGN})
endmacro(YARP_ADD_DEVICE_FINGERPRINT)



#########################################################################
# Deprecated macros
#
if(NOT YARP_NO_DEPRECATED)
include(${CMAKE_CURRENT_LIST_DIR}/YarpDeprecatedWarning.cmake)

macro(BEGIN_PLUGIN_LIBRARY)
    yarp_deprecated_warning("BEGIN_PLUGIN_LIBRARY is deprecated. Use YARP_BEGIN_PLUGIN_LIBRARY instead.")
    yarp_begin_plugin_library(${ARGN})
endmacro(BEGIN_PLUGIN_LIBRARY)

macro(ADD_PLUGIN_NORMALIZED)
    yarp_deprecated_warning("ADD_PLUGIN_NORMALIZED is deprecated. Use YARP_ADD_PLUGIN_NORMALIZED instead.")
    yarp_add_plugin_normalized(${ARGN})
endmacro(ADD_PLUGIN_NORMALIZED)

macro(PREPARE_PLUGIN)
    yarp_deprecated_warning("PREPARE_PLUGIN is deprecated. Use YARP_PREPARE_PLUGIN instead.")
    yarp_prepare_plugin(${ARGN})
endmacro(PREPARE_PLUGIN)

macro(PREPARE_DEVICE)
    yarp_deprecated_warning("PREPARE_DEVICE is deprecated. Use YARP_PREPARE_DEVICE instead.")
    yarp_prepare_device(${ARGN})
endmacro(PREPARE_DEVICE)

macro(PREPARE_CARRIER)
    yarp_deprecated_warning("PREPARE_CARRIER is deprecated. Use YARP_PREPARE_CARRIER instead.")
    yarp_prepare_carrier(${ARGN})
endmacro(PREPARE_CARRIER)

macro(END_PLUGIN_LIBRARY)
    yarp_deprecated_warning("END_PLUGIN_LIBRARY is deprecated. Use YARP_END_PLUGIN_LIBRARY instead.")
    yarp_end_plugin_library(${ARGN})
endmacro(END_PLUGIN_LIBRARY)

macro(ADD_PLUGIN_LIBRARY_EXECUTABLE)
    yarp_deprecated_warning("ADD_PLUGIN_LIBRARY_EXECUTABLE is deprecated. Use YARP_ADD_PLUGIN_LIBRARY_EXECUTABLE instead.")
    yarp_add_plugin_library_executable(${ARGN})
endmacro(ADD_PLUGIN_LIBRARY_EXECUTABLE)

endif(NOT YARP_NO_DEPRECATED)


## We skipped this whole file if it was already included
endif(NOT COMMAND YARP_END_PLUGIN_LIBRARY)
