#.rst:
# AddInstallRPATHSupport
# ----------------------
#
# Add support to RPATH during installation to your project::
#
#   add_install_rpath_support([BIN_DIRS dir [dir]]
#                             [LIB_DIRS dir [dir]]
#                             [DEPENDS condition [condition]]
#                             [USE_LINK_PATH])
#
# Normally (depending on the platform) when you install a shared
# library you can either specify its absolute path as the install name,
# or leave just the library name itself. In the former case the library
# will be correctly linked during run time by all executables and other
# shared libraries, but it must not change its install location. This
# is often the case for libraries installed in the system default
# library directory (e.g. ``/usr/lib``).
# In the latter case, instead, the library can be moved anywhere in the
# file system but at run time the dynamic linker must be able to find
# it. This is often accomplished by setting environmental variables
# (i.e. ``LD_LIBRARY_PATH`` on Linux).
# This procedure is usually not desirable for two main reasons:
#
# - by setting the variable you are changing the default behaviour
#   of the dynamic linker thus potentially breaking executables (not as
#   destructive as ``LD_PRELOAD``)
# - the variable will be used only by applications spawned by the shell
#   and not by other processes.
#
# RPATH is aimed to solve the issues introduced by the second
# installation method. Using run-path dependent libraries you can
# create a directory structure containing executables and dependent
# libraries that users can relocate without breaking it.
# A run-path dependent library is a dependent library whose complete
# install name is not known when the library is created.
# Instead, the library specifies that the dynamic loader must resolve
# the library’s install name when it loads the executable that depends
# on the library. The executable or the other shared library will
# hardcode in the binary itself the additional search directories
# to be passed to the dynamic linker. This works great in conjunction
# with relative paths.
# This command will enable support to RPATH to your project.
# It will enable the following things:
#
#  - If the project builds shared libraries it will generate a run-path
#    enabled shared library, i.e. its install name will be resolved
#    only at run time.
#  - In all cases (building executables and/or shared libraries)
#    dependent shared libraries with RPATH support will be properly
#
# The command has the following parameters:
#
# Options:
#  - ``USE_LINK_PATH``: if passed the command will automatically adds to
#    the RPATH the path to all the dependent libraries.
#
# Arguments:
#  - ``BIN_DIRS`` list of directories when the targets (executable and
#    plugins) will be installed.
#  - ``LIB_DIRS`` list of directories to be added to the RPATH. These
#    directories will be added "relative" w.r.t. the ``BIN_DIRS`` and
#    ``LIB_DIRS``.
#  - ``DEPENDS`` list of conditions that should be TRUE to enable
#    RPATH, for example ``FOO; NOT BAR``.

#=======================================================================
# Copyright 2014 RBCS, Istituto Italiano di Tecnologia
# @author Francesco Romano <francesco.romano@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=======================================================================
# (To distribute this file outside of CMake, substitute the full
# License text for the above reference.)


include(CMakeParseArguments)


function(ADD_INSTALL_RPATH_SUPPORT)

  # If RPATH is disabled in CMake, it is useless to proceed.
  if(CMAKE_SKIP_RPATH OR (CMAKE_SKIP_INSTALL_RPATH AND CMAKE_SKIP_BUILD_RPATH))
    return()
  endif()

  set(_options USE_LINK_PATH)
  set(_oneValueArgs )
  set(_multiValueArgs BIN_DIRS
                      LIB_DIRS
                      DEPENDS)

  cmake_parse_arguments(_ARS "${_options}"
                             "${_oneValueArgs}"
                             "${_multiValueArgs}"
                             "${ARGN}")

  set(_rpath_available 1)
  if(DEFINED _ARS_DEPENDS)
    foreach(_dep ${_ARS_DEPENDS})
      string(REGEX REPLACE " +" ";" _dep "${_dep}")
      if(NOT (${_dep}))
        set(_rpath_available 0)
      endif()
    endforeach()
  endif()

  if(_rpath_available)
    #Check CMake version in OS X. Required >= 2.8.12
    if(CMAKE_VERSION VERSION_LESS 2.8.12 AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      message(WARNING "Your CMake version is too old. RPATH support on OS X requires CMake version at least 2.8.12")
    endif()

    # Enable RPATH on OSX. This also suppress warnings on CMake >= 3.0
    set(CMAKE_MACOSX_RPATH TRUE PARENT_SCOPE)

    # If install RPATH is disabled in CMake, it is useless to evaluate and set
    # CMAKE_INSTALL_RPATH and CMAKE_INSTALL_RPATH_USE_LINK_PATH
    if(CMAKE_SKIP_INSTALL_RPATH)
      return()
    endif()

    # Find system implicit lib directories
    set(_system_lib_dirs ${CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES})
    if(EXISTS "/etc/debian_version") # is this a debian system ?
        if(CMAKE_LIBRARY_ARCHITECTURE)
            list(APPEND _system_lib_dirs "/lib/${CMAKE_LIBRARY_ARCHITECTURE}"
                                         "/usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}")
        endif()
    endif()
    # This is relative RPATH for libraries built in the same project
    foreach(lib_dir ${_ARS_LIB_DIRS})
      list(FIND _system_lib_dirs "${lib_dir}" isSystemDir)
      if("${isSystemDir}" STREQUAL "-1")
        foreach(bin_dir ${_ARS_LIB_DIRS} ${_ARS_BIN_DIRS})
          file(RELATIVE_PATH _rel_path ${bin_dir} ${lib_dir})
          if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            list(APPEND CMAKE_INSTALL_RPATH "@loader_path/${_rel_path}")
          else()
            list(APPEND CMAKE_INSTALL_RPATH "\$ORIGIN/${_rel_path}")
          endif()
        endforeach()
      endif()
    endforeach()
    if(NOT "${CMAKE_INSTALL_RPATH}" STREQUAL "")
      list(REMOVE_DUPLICATES CMAKE_INSTALL_RPATH)
    endif()
    set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH} PARENT_SCOPE)

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ${_ARS_USE_LINK_PATH} PARENT_SCOPE)

  endif()

endfunction()
