# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Alessandro Scalzo, Daniele E. Domenichelli
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

#########################################################################
# Check whether system is big- or little- endian

include(TestBigEndian)
test_big_endian(IS_BIG_ENDIAN)
if(${IS_BIG_ENDIAN})
    set(YARP_BIG_ENDIAN 1)
else(${IS_BIG_ENDIAN})
    set(YARP_LITTLE_ENDIAN 1)
endif(${IS_BIG_ENDIAN})

#########################################################################
# Find 16, 32, and 64 bit types, portably

include(CheckTypeSize)

set(YARP_INT16)
set(YARP_INT32)
set(YARP_INT64)
set(YARP_FLOAT64)

check_type_size("short" SIZEOF_SHORT)
check_type_size("int" SIZEOF_INT)
check_type_size("long" SIZEOF_LONG)
if(SIZEOF_INT EQUAL 4)
    set(YARP_INT32 "int")
else(SIZEOF_INT EQUAL 4)
    if(SIZEOF_SHORT EQUAL 4)
        set(YARP_INT32 "short")
    else(SIZEOF_SHORT EQUAL 4)
        if(SIZEOF_LONG EQUAL 4)
            set(YARP_INT32 "long")
        endif(SIZEOF_LONG EQUAL 4)
    endif(SIZEOF_SHORT EQUAL 4)
endif(SIZEOF_INT EQUAL 4)

if(SIZEOF_SHORT EQUAL 2)
    set(YARP_INT16 "short")
else(SIZEOF_SHORT EQUAL 2)
    # Hmm - there's no other native type to get 16 bits
    # We will continue since most people using YARP do not need one.
    message(STATUS "Warning: cannot find a 16 bit type on your system")
    message(STATUS "Continuing...")
endif(SIZEOF_SHORT EQUAL 2)

check_type_size("float" SIZEOF_FLOAT)
check_type_size("double" SIZEOF_DOUBLE)
if(SIZEOF_DOUBLE EQUAL 8)
    set(YARP_FLOAT64 "double")
else(SIZEOF_DOUBLE EQUAL 8)
    if(SIZEOF_FLOAT EQUAL 8)
        set(YARP_FLOAT64 "float")
    endif(SIZEOF_FLOAT EQUAL 8)
endif(SIZEOF_DOUBLE EQUAL 8)

if(SIZEOF_DOUBLE EQUAL 4)
    set(YARP_FLOAT32 "double")
else(SIZEOF_DOUBLE EQUAL 4)
    if(SIZEOF_FLOAT EQUAL 4)
        set(YARP_FLOAT32 "float")
    endif(SIZEOF_FLOAT EQUAL 4)
endif(SIZEOF_DOUBLE EQUAL 4)

if(SIZEOF_LONG EQUAL 8)
    set(YARP_INT64 "long")
else(SIZEOF_LONG EQUAL 8)
    check_type_size("long long" SIZEOF_LONGLONG)
    if(SIZEOF_LONGLONG EQUAL 8)
        set(YARP_INT64 "long long")
    else(SIZEOF_LONGLONG EQUAL 8)
        check_type_size("__int64" SIZEOF___INT64)
        if(SIZEOF___INT64 EQUAL 8)
            set(YARP_INT64 "__int64")
        endif(SIZEOF___INT64 EQUAL 8)
    endif(SIZEOF_LONGLONG EQUAL 8)
endif(SIZEOF_LONG EQUAL 8)

#########################################################################
# Set up compile flags

add_definitions(-DYARP_PRESENT)
add_definitions(-D_REENTRANT)
# due to cmake 2.6.0 bug, cannot use APPEND here
# set_property(GLOBAL APPEND PROPERTY YARP_DEFS -D_REENTRANT)
set_property(GLOBAL PROPERTY YARP_DEFS -D_REENTRANT)

# on windows, we have to tell ace how it was compiled
if(WIN32)
    ## check if we are using the CYGWIN compiler
    if(NOT CYGWIN)
        add_definitions(-DWIN32 -D_WINDOWS)
    else(NOT CYGWIN)
        add_definitions(-DCYGWIN)
    endif(NOT CYGWIN)

    ## check if we are using the MINGW compiler
    if(MINGW)
        add_definitions(-D__MINGW__ -D__MINGW32__ "-mms-bitfields" "-mthreads" "-Wpointer-arith" "-pipe")
        # "-fno-exceptions" can be useful too... unless you need exceptions :-)
        if(MSYS)
            add_definitions(-D__ACE_INLINE__ -DACE_HAS_ACE_TOKEN -DACE_HAS_ACE_SVCCONF -DACE_BUILD_DLL)
        else(MSYS)
            add_definitions("-fvisibility=hidden" "-fvisibility-inlines-hidden" "-Wno-attributes")
        endif(MSYS)
    endif(MINGW)

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
    endif(MSVC)
else(WIN32)

    include(CheckCXXCompilerFlag)

    ## Wanted warnings flags ##

    set(CMAKE_CXX_FLAGS "-Wall -Wextra")

    check_cxx_compiler_flag("-Wsign-compare" CXX_HAS_WSIGN_COMPARE)
    if(CXX_HAS_WSIGN_COMPARE)
        set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} -Wsign-compare")
    endif (CXX_HAS_WSIGN_COMPARE)

    check_cxx_compiler_flag("-Wpointer-arith" CXX_HAS_WPOINTER_ARITH)
    if(CXX_HAS_WPOINTER_ARITH)
        set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} -Wpointer-arith")
    endif(CXX_HAS_WPOINTER_ARITH)

    check_cxx_compiler_flag("-Wformat" CXX_HAS_WFORMAT)
    if(CXX_HAS_WFORMAT)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat")
        check_cxx_compiler_flag("-Wformat-security" CXX_HAS_WFORMAT_SECURITY)
        if(CXX_HAS_WFORMAT_SECURITY)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-security")
        endif(CXX_HAS_WFORMAT_SECURITY)
    endif(CXX_HAS_WFORMAT)

    check_cxx_compiler_flag("-Winit-self" CXX_HAS_WINIT_SELF)
    if(CXX_HAS_WINIT_SELF)
        set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} -Winit-self")
    endif(CXX_HAS_WINIT_SELF)

    check_cxx_compiler_flag("-Wnon-virtual-dtor" CXX_HAS_WNON_VIRTUAL_DTOR)
    if(CXX_HAS_WNON_VIRTUAL_DTOR)
        set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} -Wnon-virtual-dtor")
    endif(CXX_HAS_WNON_VIRTUAL_DTOR)

    check_cxx_compiler_flag("-Wcast-align" CXX_HAS_WCAST_ALIGN)
    if(CXX_HAS_WCAST_ALIGN)
        set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} -Wcast-align")
    endif(CXX_HAS_WCAST_ALIGN)

#    check_cxx_compiler_flag("-Wundef" CXX_HAS_WUNDEF)
#    if(CXX_HAS_WUNDEF)
#        set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} -Wundef")
#    endif(CXX_HAS_WUNDEF)

    check_cxx_compiler_flag("-Woverloaded-virtual" CXX_HAS_WOVERLOADED_VIRTUAL)
    if(CXX_HAS_WOVERLOADED_VIRTUAL)
        set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} -Woverloaded-virtual")
    endif(CXX_HAS_WOVERLOADED_VIRTUAL)



    ## Unwanted warning flags ##

    check_cxx_compiler_flag("-Wno-unused-parameter" CXX_HAS_WNO_UNUSED_PARAMETER)
    if(CXX_HAS_WNO_UNUSED_PARAMETER)
        set(UNWANTED_WARNING_FLAGS "${UNWANTED_WARNING_FLAGS} -Wno-unused-parameter")
    endif(CXX_HAS_WNO_UNUSED_PARAMETER)

    check_cxx_compiler_flag("-Wno-long-long" CXX_HAS_WNO_LONG_LONG)
    if(CXX_HAS_WNO_LONG_LONG)
       set(UNWANTED_WARNING_FLAGS "${UNWANTED_WARNING_FLAGS} -Wno-long-long")
    endif(CXX_HAS_WNO_LONG_LONG)



    ## Visibility hidden flags ##

    check_cxx_compiler_flag("-fvisibility=hidden" CXX_HAS_FVISIBILITY_HIDDEN)
    if(CXX_HAS_FVISIBILITY_HIDDEN)
        set(VISIBILITY_HIDDEN_FLAGS "-fvisibility=hidden")
    else(CXX_HAS_FVISIBILITY_HIDDEN)
        set(VISIBILITY_HIDDEN_FLAGS)
    endif(CXX_HAS_FVISIBILITY_HIDDEN)

    check_cxx_compiler_flag("-fvisibility-inlines-hidden" CXX_HAS_FVISIBILITY_INLINES_HIDDEN)
    if(CXX_HAS_FVISIBILITY_INLINES_HIDDEN)
        set(VISIBILITY_HIDDEN_FLAGS "${VISIBILITY_HIDDEN_FLAGS} -fvisibility-inlines-hidden")
    endif(CXX_HAS_FVISIBILITY_INLINES_HIDDEN)



    ## Deprcated declarations flags ##

    check_cxx_compiler_flag("-Wdeprecated-declarations" CXX_HAS_WDEPRECATED_DECLARATIONS)
    if(CXX_HAS_WDEPRECATED_DECLARATIONS)
        set(DEPRECATED_DECLARATIONS_FLAGS "-Wdeprecated-declarations")
    else(CXX_HAS_WDEPRECATED_DECLARATIONS)
        set(DEPRECATED_DECLARATIONS_FLAGS)
    endif(CXX_HAS_WDEPRECATED_DECLARATIONS)

endif(WIN32)


# Translate the names of some YARP options, for yarp_config_options.h.in
# and YARPConfig.cmake.in
set (YARP_HAS_MATH_LIB ${CREATE_LIB_MATH})
set (YARP_HAS_NAME_LIB ${YARP_USE_PERSISTENT_NAMESERVER})
