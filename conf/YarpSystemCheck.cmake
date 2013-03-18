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
set(YARP_FLOAT32)
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

check_type_size("void *" YARP_POINTER_SIZE)

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

    set(WANTED_WARNING_FLAGS "-Wall -Wextra")

    check_cxx_compiler_flag("-Wsign-compare" CXX_HAS_WSIGN_COMPARE)
    if(CXX_HAS_WSIGN_COMPARE)
        set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} -Wsign-compare")
    endif (CXX_HAS_WSIGN_COMPARE)

    check_cxx_compiler_flag("-Wpointer-arith" CXX_HAS_WPOINTER_ARITH)
    if(CXX_HAS_WPOINTER_ARITH)
        set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} -Wpointer-arith")
    endif(CXX_HAS_WPOINTER_ARITH)

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

    check_cxx_compiler_flag("-Wunused" CXX_HAS_UNUSED)
    if(CXX_HAS_WUNUSED)
        set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} -Wunused")
    endif(CXX_HAS_WUNUSED)

    check_cxx_compiler_flag("-Wvla" CXX_HAS_WVLA)
    if(CXX_HAS_WVLA)
        set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} -Wvla")
    endif(CXX_HAS_WVLA)



    ## Unwanted warning flags ##

    set(UNWANTED_WARNING_FLAGS)

    # FIXME this warning should be enabled later.
    check_cxx_compiler_flag("-Wno-unused-parameter" CXX_HAS_WNO_UNUSED_PARAMETER)
    if(CXX_HAS_WNO_UNUSED_PARAMETER)
        set(UNWANTED_WARNING_FLAGS "${UNWANTED_WARNING_FLAGS} -Wno-unused-parameter")
    endif(CXX_HAS_WNO_UNUSED_PARAMETER)

    check_cxx_compiler_flag("-Wno-long-long" CXX_HAS_WNO_LONG_LONG)
    if(CXX_HAS_WNO_LONG_LONG)
        set(UNWANTED_WARNING_FLAGS "${UNWANTED_WARNING_FLAGS} -Wno-long-long")
    endif(CXX_HAS_WNO_LONG_LONG)



    ## Experimental warning flags ##
    # FIXME Those warnings should be enabled later

    set(EXPERIMENTAL_WARNING_FLAGS)

    check_cxx_compiler_flag("-Wundef" CXX_HAS_WUNDEF)
    if(CXX_HAS_WUNDEF)
        set(EXPERIMENTAL_WARNING_FLAGS "${EXPERIMENTAL_WARNING_FLAGS} -Wundef")
    endif(CXX_HAS_WUNDEF)

    check_cxx_compiler_flag("-Woverloaded-virtual" CXX_HAS_WOVERLOADED_VIRTUAL)
    if(CXX_HAS_WOVERLOADED_VIRTUAL)
        set(EXPERIMENTAL_WARNING_FLAGS "${EXPERIMENTAL_WARNING_FLAGS} -Woverloaded-virtual")
    endif(CXX_HAS_WOVERLOADED_VIRTUAL)

    check_cxx_compiler_flag("-Wconversion" CXX_HAS_WCONVERSION)
    if(CXX_HAS_WCONVERSION)
        set(EXPERIMENTAL_WARNING_FLAGS "${EXPERIMENTAL_WARNING_FLAGS} -Wconversion")
    endif(CXX_HAS_WCONVERSION)

    check_cxx_compiler_flag("-Wsign-conversion" CXX_HAS_WSIGN_CONVERSION)
    if(CXX_HAS_WSIGN_CONVERSION)
        set(EXPERIMENTAL_WARNING_FLAGS "${EXPERIMENTAL_WARNING_FLAGS} -Wsign-conversion")
    endif(CXX_HAS_WSIGN_CONVERSION)

    check_cxx_compiler_flag("-Wold-style-cast" CXX_HAS_WOLD_STYLE_CAST)
    if(CXX_HAS_WOLD_STYLE_CAST)
        set(EXPERIMENTAL_WARNING_FLAGS "${EXPERIMENTAL_WARNING_FLAGS} -Wold-style-cast")
    endif(CXX_HAS_WOLD_STYLE_CAST)

    check_cxx_compiler_flag("-Wredundant-decls" CXX_HAS_WREDUNDANT_DECLS)
    if(CXX_HAS_WREDUNDANT_DECLS)
        set(EXPERIMENTAL_WARNING_FLAGS "${EXPERIMENTAL_WARNING_FLAGS} -Wredundant-decls")
    endif(CXX_HAS_WREDUNDANT_DECLS)

    check_cxx_compiler_flag("-Winline" CXX_HAS_WINLINE)
    if(CXX_HAS_WINLINE)
        set(EXPERIMENTAL_WARNING_FLAGS "${EXPERIMENTAL_WARNING_FLAGS} -Winline")
    endif(CXX_HAS_WINLINE)



    ## Visibility hidden flags ##

    set(VISIBILITY_HIDDEN_FLAGS)

    check_cxx_compiler_flag("-fvisibility=hidden" CXX_HAS_FVISIBILITY_HIDDEN)
    if(CXX_HAS_FVISIBILITY_HIDDEN)
        set(VISIBILITY_HIDDEN_FLAGS "-fvisibility=hidden")
    endif(CXX_HAS_FVISIBILITY_HIDDEN)

    check_cxx_compiler_flag("-fvisibility-inlines-hidden" CXX_HAS_FVISIBILITY_INLINES_HIDDEN)
    if(CXX_HAS_FVISIBILITY_INLINES_HIDDEN)
        set(VISIBILITY_HIDDEN_FLAGS "${VISIBILITY_HIDDEN_FLAGS} -fvisibility-inlines-hidden")
    endif(CXX_HAS_FVISIBILITY_INLINES_HIDDEN)



    ## Deprcated declarations flags ##

    set(DEPRECATED_DECLARATIONS_FLAGS)

    check_cxx_compiler_flag("-Wdeprecated-declarations" CXX_HAS_WDEPRECATED_DECLARATIONS)
    if(CXX_HAS_WDEPRECATED_DECLARATIONS)
        set(DEPRECATED_DECLARATIONS_FLAGS "-Wdeprecated-declarations")
    endif(CXX_HAS_WDEPRECATED_DECLARATIONS)



    ## Hardening flags ##

    set(HARDENING_FLAGS)

    check_cxx_compiler_flag("-Wformat" CXX_HAS_WFORMAT)
    if(CXX_HAS_WFORMAT)

        check_cxx_compiler_flag("-Wformat=2" CXX_HAS_WFORMAT_2)
        if(CXX_HAS_WFORMAT_2)
            set(HARDENING_FLAGS "-Wformat=2")
        else(CXX_HAS_WFORMAT_2)
            set(HARDENING_FLAGS "-Wformat")
        endif(CXX_HAS_WFORMAT_2)

        check_cxx_compiler_flag("-Wformat-security -Werror=format-security" CXX_HAS_WFORMAT_SECURITY)
        if(CXX_HAS_WFORMAT_SECURITY)
            set(HARDENING_FLAGS "${HARDENING_FLAGS} -Wformat-security -Werror=format-security")
        endif(CXX_HAS_WFORMAT_SECURITY)

        check_cxx_compiler_flag("-Wformat-y2k" CXX_HAS_WFORMAT_Y2K)
        if(CXX_HAS_WFORMAT_Y2K)
            set(HARDENING_FLAGS "${HARDENING_FLAGS} -Wformat-y2k")
        endif(CXX_HAS_WFORMAT_Y2K)

        check_cxx_compiler_flag("-Wformat-nonliteral" CXX_HAS_WFORMAT_NONLITERAL)
        if(CXX_HAS_WFORMAT_NONLITERAL)
            set(HARDENING_FLAGS "${HARDENING_FLAGS} -Wformat-nonliteral")
        endif(CXX_HAS_WFORMAT_NONLITERAL)

    endif(CXX_HAS_WFORMAT)

    check_cxx_compiler_flag("-fstack-protector --param=ssp-buffer-size=4" CXX_HAS_STACK_PROTECTOR)
    if(CXX_HAS_STACK_PROTECTOR)
        set(HARDENING_FLAGS "${HARDENING_FLAGS} -fstack-protector --param=ssp-buffer-size=4")
    endif(CXX_HAS_STACK_PROTECTOR)

    check_cxx_compiler_flag("-Wl,-zrelro" CXX_HAS_WL__ZRELRO)
    if(CXX_HAS_WL__ZRELRO)
       set(HARDENING_FLAGS "${HARDENING_FLAGS} -Wl,-zrelro")
    endif(CXX_HAS_WL__ZRELRO)

    check_cxx_compiler_flag("-Wl,-znow" CXX_HAS_WL__ZNOW)
    if(CXX_HAS_WL__ZNOW)
        set(HARDENING_FLAGS "${HARDENING_FLAGS} -Wl,-znow")
    endif(CXX_HAS_WL__ZNOW)

    check_cxx_compiler_flag("-fPIE -pie" CXX_HAS_FPIE)
    if(CXX_HAS_FPIE)
        set(HARDENING_FLAGS "${HARDENING_FLAGS} -fPIE -pie")
    endif(CXX_HAS_FPIE)

    ## C++11 flags

    check_cxx_compiler_flag("-std=c++11" CXX_HAS_STD_CXX11)
    check_cxx_compiler_flag("-std=c++0x" CXX_HAS_STD_CXX0X)
    if(CXX_HAS_STD_CXX11)
        set(CXX11_FLAGS "-std=c++11")
        check_cxx_compiler_flag("-Wc++11-compat" CXX_HAS_CXX11_COMPAT)
        if(CXX_HAS_CXX11_COMPAT)
            set(CXX11_FLAGS "${CXX11_FLAGS} -Wc++11-compat")
        endif(CXX_HAS_CXX11_COMPAT)
    elseif(CXX_HAS_STD_CXX0X)
        set(CXX11_FLAGS "-std=c++11")
    else(CXX_HAS_STD_CXX11)
        set(CXX11_FLAGS)
    endif(CXX_HAS_STD_CXX11)


    include (CheckIncludeFiles)
    check_include_files (execinfo.h YARP_HAS_EXECINFO)

endif(WIN32)


# Translate the names of some YARP options, for yarp_config_options.h.in
# and YARPConfig.cmake.in
set (YARP_HAS_MATH_LIB ${CREATE_LIB_MATH})
set (YARP_HAS_NAME_LIB ${YARP_USE_PERSISTENT_NAMESERVER})
