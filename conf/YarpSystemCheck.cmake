# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Alessandro Scalzo, Daniele E. Domenichelli
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


#########################################################################
# Include required CMake modules

include(TestBigEndian)
include(CheckCXXCompilerFlag)
include(CheckIncludeFiles)

# CheckTypeSize changes CMAKE_MINIMUM_REQUIRED_VERSION, see
# http://www.cmake.org/Bug/view.php?id=14864 (fixed in CMake 3.1)
# We save it here, and restore it after including the file.
if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.1)
  message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
endif()
if(CMAKE_VERSION VERSION_LESS 3.1)
  set(_CMAKE_MINIMUM_REQUIRED_VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})
endif()
include(CheckTypeSize)
if(CMAKE_VERSION VERSION_LESS 3.1)
  cmake_minimum_required(VERSION ${_CMAKE_MINIMUM_REQUIRED_VERSION})
endif()

# Ensure that install directories are set
include(GNUInstallDirs)


#########################################################################
# Check whether system is big- or little- endian

test_big_endian(IS_BIG_ENDIAN)
if(${IS_BIG_ENDIAN})
  set(YARP_BIG_ENDIAN 1)
else()
  set(YARP_LITTLE_ENDIAN 1)
endif()


#########################################################################
# Find 16, 32, and 64 bit types, portably

set(YARP_INT16)
set(YARP_INT32)
set(YARP_INT64)
set(YARP_FLOAT32)
set(YARP_FLOAT64)

check_type_size("short" SIZEOF_SHORT)
check_type_size("int" SIZEOF_INT)
check_type_size("long" SIZEOF_LONG)
if(SIZEOF_INT EQUAL 4)
  set(YARP_INT32 "int")
  set(YARP_INT32_FMT "d")
else()
  if(SIZEOF_SHORT EQUAL 4)
    set(YARP_INT32 "short")
    set(YARP_INT32_FMT "hd")
  elseif(SIZEOF_LONG EQUAL 4)
    set(YARP_INT32 "long")
    set(YARP_INT32_FMT "ld")
  endif()
endif()

if(SIZEOF_SHORT EQUAL 2)
  set(YARP_INT16 "short")
else()
  # Hmm - there's no other native type to get 16 bits
  # We will continue since most people using YARP do not need one.
  message(STATUS "Warning: cannot find a 16 bit type on your system")
  message(STATUS "Continuing...")
endif()

check_type_size("float" SIZEOF_FLOAT)
check_type_size("double" SIZEOF_DOUBLE)
if(SIZEOF_DOUBLE EQUAL 8)
  set(YARP_FLOAT64 "double")
elseif(SIZEOF_FLOAT EQUAL 8)
  set(YARP_FLOAT64 "float")
endif()

if(SIZEOF_DOUBLE EQUAL 4)
  set(YARP_FLOAT32 "double")
elseif(SIZEOF_FLOAT EQUAL 4)
  set(YARP_FLOAT32 "float")
endif()

if(SIZEOF_LONG EQUAL 8)
  set(YARP_INT64 "long")
  set(YARP_INT64_FMT "ld")
else()
  check_type_size("long long" SIZEOF_LONGLONG)
  if(SIZEOF_LONGLONG EQUAL 8)
    set(YARP_INT64 "long long")
  else()
    check_type_size("__int64" SIZEOF___INT64)
    if(SIZEOF___INT64 EQUAL 8)
      set(YARP_INT64 "__int64")
    endif()
  endif()
  set(YARP_INT64_FMT "lld")
endif()

check_type_size("void *" YARP_POINTER_SIZE)


set(YARP_SSIZE_T int)
check_type_size(ssize_t YARP_SSIZE_T_LOWER)
if(HAVE_YARP_SSIZE_T_LOWER)
  set(YARP_SSIZE_T ssize_t)
else()
  check_type_size(SSIZE_T YARP_SSIZE_T_HIGHER)
  if(HAVE_YARP_SSIZE_T_HIGHER)
    set(YARP_SSIZE_T SSIZE_T)
  else()
    check_type_size(size_t YARP_SIZE_T)
    if(YARP_SIZE_T EQUAL 8)
      set(YARP_SSIZE_T ${YARP_INT64})
    elseif(YARP_SIZE_T EQUAL 4)
      set(YARP_SSIZE_T ${YARP_INT32})
    elseif(YARP_SIZE_T EQUAL 2)
      set(YARP_SSIZE_T ${YARP_INT16})
    endif()
  endif()
endif()

set(YARP_HAVE_SYS_TYPES_H ${HAVE_SYS_TYPES_H})
set(YARP_HAVE_STDDEF_H ${HAVE_STDDEF_H})


#########################################################################
# Set up compile flags

add_definitions(-DYARP_PRESENT)
add_definitions(-D_REENTRANT)
set_property(GLOBAL APPEND PROPERTY YARP_DEFS -D_REENTRANT)

# on windows, we have to tell ace how it was compiled
if(WIN32)
  ## check if we are using the CYGWIN compiler
  if(NOT CYGWIN)
    add_definitions(-DWIN32 -D_WINDOWS)
  else()
    add_definitions(-DCYGWIN)
  endif()

  ## check if we are using the MINGW compiler
  if(MINGW)
    add_definitions(-D__MINGW__ -D__MINGW32__ "-mms-bitfields" "-mthreads" "-Wpointer-arith" "-pipe")
    # "-fno-exceptions" can be useful too... unless you need exceptions :-)
    if(MSYS)
      add_definitions(-D__ACE_INLINE__ -DACE_HAS_ACE_TOKEN -DACE_HAS_ACE_SVCCONF -DACE_BUILD_DLL)
    else()
      add_definitions("-fvisibility=hidden" "-fvisibility-inlines-hidden" "-Wno-attributes")
    endif()
  endif()

  ## check if we are using the MSVC compiler
  if(MSVC)
    # ACE uses a bunch of functions MSVC warns about.
    # The warnings make sense in general, but not in this case.
    # this gets rids of deprecated unsafe crt functions
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    # this gets rid of warning about deprecated POSIX names
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)

    # disable: warning C4355: 'this' : used ...
    ## this never worked, giving up.
    #add_definitions(/wd4355)

    # Traditionally, we add "d" postfix to debug libraries
    set(CMAKE_DEBUG_POSTFIX "d")
  endif()
else()

    if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.0)
      message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. The check in this macro can be removed.")
    endif()
    macro(YARP_CHECK_AND_APPEND_CXX_COMPILER_FLAG _out _flag)
      string(TOUPPER "${_flag}" _VAR)
      string(REGEX REPLACE " .+" "" _VAR "${_VAR}")
      string(REGEX REPLACE "[^A-Za-z0-9]" "_" _VAR "${_VAR}")
      set(_VAR CXX_HAS${_VAR})
      check_cxx_compiler_flag("${_flag}" ${_VAR})
      if(${_VAR})
        if(NOT CMAKE_VERSION VERSION_LESS 3.0)
          string(CONCAT ${_out} ${_flag})
        else()
          set(${_out} "${${_out}} ${_flag}")
        endif()
      endif()
      unset(_VAR)
    endmacro()

    ## Wanted warnings flags ##
    unset(WANTED_WARNING_FLAGS)
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wall")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wextra")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wsign-compare")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wpointer-arith")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Winit-self")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wnon-virtual-dtor")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wcast-align")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wunused")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wunused-but-set-variable")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wvla")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wmissing-include-dirs")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wlogical-op")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wreorder")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wsizeof-pointer-memaccess")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Woverloaded-virtual")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wc++11-compat")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wtautological-undefined-compare")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wmismatched-new-delete")
    yarp_check_and_append_cxx_compiler_flag(WANTED_WARNING_FLAGS "-Wparentheses-equality")

    ## Unwanted warning flags ##
    unset(UNWANTED_WARNING_FLAGS)
    yarp_check_and_append_cxx_compiler_flag(UNWANTED_WARNING_FLAGS "-Wno-unused-parameter") # FIXME Enable later
    yarp_check_and_append_cxx_compiler_flag(UNWANTED_WARNING_FLAGS "-Wno-long-long")
    yarp_check_and_append_cxx_compiler_flag(UNWANTED_WARNING_FLAGS "-Wno-cast-align") # FIXME Enable later


    ## Experimental warning flags ##
    # FIXME Those warnings should be enabled later
    unset(EXPERIMENTAL_WARNING_FLAGS)
    yarp_check_and_append_cxx_compiler_flag(EXPERIMENTAL_WARNING_FLAGS "-Wundef")
    yarp_check_and_append_cxx_compiler_flag(EXPERIMENTAL_WARNING_FLAGS "-Wconversion")
    yarp_check_and_append_cxx_compiler_flag(EXPERIMENTAL_WARNING_FLAGS "-Wsign-conversion")
    yarp_check_and_append_cxx_compiler_flag(EXPERIMENTAL_WARNING_FLAGS "-Wold-style-cast")
    yarp_check_and_append_cxx_compiler_flag(EXPERIMENTAL_WARNING_FLAGS "-Wredundant-decls")
    yarp_check_and_append_cxx_compiler_flag(EXPERIMENTAL_WARNING_FLAGS "-Winline")
    yarp_check_and_append_cxx_compiler_flag(EXPERIMENTAL_WARNING_FLAGS "-Wfloat-equal")


    ## Visibility hidden flags ##
    unset(VISIBILITY_HIDDEN_FLAGS)
    yarp_check_and_append_cxx_compiler_flag(VISIBILITY_HIDDEN_FLAGS "-fvisibility=hidden")
    yarp_check_and_append_cxx_compiler_flag(VISIBILITY_HIDDEN_FLAGS "-fvisibility-inlines-hidden")


    ## Deprcated declarations flags ##
    unset(DEPRECATED_DECLARATIONS_FLAGS)
    yarp_check_and_append_cxx_compiler_flag(DEPRECATED_DECLARATIONS_FLAGS "-Wdeprecated-declarations")


    ## Hardening flags ##
    unset(HARDENING_FLAGS)
    check_cxx_compiler_flag("-Wformat" CXX_HAS_WFORMAT)
    if(CXX_HAS_WFORMAT)
      check_cxx_compiler_flag("-Wformat=2" CXX_HAS_WFORMAT_2)
      if(CXX_HAS_WFORMAT_2)
        set(HARDENING_FLAGS "-Wformat=2")
      else()
        set(HARDENING_FLAGS "-Wformat")
      endif()
      yarp_check_and_append_cxx_compiler_flag(HARDENING_FLAGS "-Wformat-security")
      yarp_check_and_append_cxx_compiler_flag(HARDENING_FLAGS "-Wformat-y2k")
      yarp_check_and_append_cxx_compiler_flag(HARDENING_FLAGS "-Wformat-nonliteral")
    endif()
    yarp_check_and_append_cxx_compiler_flag(HARDENING_FLAGS "-fstack-protector --param=ssp-buffer-size=4")
    yarp_check_and_append_cxx_compiler_flag(HARDENING_FLAGS "-Wl,-zrelro")
    yarp_check_and_append_cxx_compiler_flag(HARDENING_FLAGS "-Wl,-znow")
    yarp_check_and_append_cxx_compiler_flag(HARDENING_FLAGS "-fPIE -pie")


    ## C++11 flags ##
    unset(CXX11_FLAGS)
    check_cxx_compiler_flag("-std=c++11" CXX_HAS_STD_CXX11)
    check_cxx_compiler_flag("-std=c++0x" CXX_HAS_STD_CXX0X)
    if(CXX_HAS_STD_CXX11)
      set(CXX11_FLAGS "-std=c++11")
    elseif(CXX_HAS_STD_CXX0X)
      set(CXX11_FLAGS "-std=c++0x")
    endif()


    ## C++14 flags ##
    unset(CXX14_FLAGS)
    check_cxx_compiler_flag("-std=c++1y" CXX_HAS_STD_CXX1Y)
    if(CXX_HAS_STD_CXX11)
      set(CXX14_FLAGS "-std=c++1y")
    endif()

endif()


# Try to locate execinfo.h
check_include_files(execinfo.h YARP_HAS_EXECINFO)


# Translate the names of some YARP options, for yarp_config_options.h.in
# and YARPConfig.cmake.in
set(YARP_HAS_MATH_LIB ${CREATE_LIB_MATH})
set(YARP_HAS_NAME_LIB ${YARP_USE_PERSISTENT_NAMESERVER})

#########################################################################
# Tweak tests for MSVC, to add paths to DLLs
if(MSVC)
  configure_file(${YARP_MODULE_DIR}/template/TestConfig.cmake ${CMAKE_BINARY_DIR}/TestConfig.cmake @ONLY)
  set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY TEST_INCLUDE_FILE ${CMAKE_BINARY_DIR}/TestConfig.cmake)
endif()
