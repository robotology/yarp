# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Alessandro Scalzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


#########################################################################
# Configure ACE

option(SKIP_ACE "Compile YARP without ACE (Linux only, TCP only, limited functionality)" FALSE)
mark_as_advanced(SKIP_ACE)


if(SKIP_ACE)
    set(ACE_LIBRARIES pthread rt)
else(SKIP_ACE)
    set(YARP_HAS_ACE 1)

    find_package(ACE REQUIRED)

    # From ACE 6.0.2 on, __ACE_INLINE__ is needed in some configurations
    include (CheckCXXSourceCompiles)
    file(READ ${YARP_MODULE_DIR}/ace_test.cpp YARP_ACE_NEEDS_INLINE_CPP)
    set(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIRS})
    if(ACE_ACE_LIBRARY_RELEASE)
        set(CMAKE_REQUIRED_LIBRARIES ${ACE_ACE_LIBRARY_RELEASE})
        check_cxx_source_compiles("${YARP_ACE_NEEDS_INLINE_CPP}" YARP_ACE_COMPILES_WITHOUT_INLINE_RELEASE)
    endif()
    if(ACE_ACE_LIBRARY_DEBUG)
        set(CMAKE_REQUIRED_LIBRARIES ${ACE_ACE_LIBRARY_DEBUG})
        set(CMAKE_REQUIRED_DEFINITIONS "-D_DEBUG")
        check_cxx_source_compiles("${YARP_ACE_NEEDS_INLINE_CPP}" YARP_ACE_COMPILES_WITHOUT_INLINE_DEBUG)
    endif()
    unset(CMAKE_REQUIRED_INCLUDES)
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_DEFINITIONS)

    if(NOT YARP_ACE_COMPILES_WITHOUT_INLINE_RELEASE)
        foreach(_config ${YARP_OPTIMIZED_CONFIGURATIONS})
            string(TOUPPER ${_config} _CONFIG)
            set(CMAKE_C_FLAGS_${_CONFIG} "${CMAKE_C_FLAGS_${_CONFIG}} -D__ACE_INLINE__")
            set(CMAKE_CXX_FLAGS_${_CONFIG} "${CMAKE_CXX_FLAGS_${_CONFIG}} -D__ACE_INLINE__")
        endforeach()
    endif()

    if(NOT YARP_ACE_COMPILES_WITHOUT_INLINE_DEBUG)
        foreach(_config ${YARP_DEBUG_CONFIGURATIONS})
            string(TOUPPER ${_config} _CONFIG)
            set(CMAKE_C_FLAGS_${_CONFIG} "${CMAKE_C_FLAGS_${_CONFIG}} -D__ACE_INLINE__")
            set(CMAKE_CXX_FLAGS_${_CONFIG} "${CMAKE_CXX_FLAGS_${_CONFIG}} -D__ACE_INLINE__")
        endforeach()
    endif()

    if(NOT YARP_ACE_COMPILES_WITHOUT_INLINE_RELEASE)
        set(YARP_ACE_REQUIRED_DEFINITIONS "-D__ACE_INLINE__")
    endif()

    include(YarpCheckTypeSize) # regular script does not do C++ types
    set(CMAKE_EXTRA_INCLUDE_FILES ace/config.h ace/String_Base_Const.h)
    set(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIRS})
    set(CMAKE_REQUIRED_LIBRARIES ${ACE_LIBRARIES})
    set(CMAKE_REQUIRED_DEFINITIONS ${YARP_ACE_REQUIRED_DEFINITIONS})
    yarp_check_type_size(ACE_String_Base_Const::size_type SIZE_TYPE)
    unset(CMAKE_EXTRA_INCLUDE_FILES)
    unset(CMAKE_REQUIRED_INCLUDES)
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_DEFINITIONS)
    set(YARP_USE_ACE_STRING_BASE_CONST_SIZE_TYPE ${HAVE_SIZE_TYPE})

    include(YarpCheckStructHasMember)
    set(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIRS})
    set(CMAKE_REQUIRED_LIBRARIES ${ACE_LIBRARIES})
    set(CMAKE_REQUIRED_DEFINITIONS ${YARP_ACE_REQUIRED_DEFINITIONS})
    yarp_check_struct_has_member("ACE_INET_Addr" is_loopback ace/INET_Addr.h YARP_ACE_ADDR_HAS_LOOPBACK_METHOD)
    unset(CMAKE_REQUIRED_INCLUDES)
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_DEFINITIONS)

    # With migration to std::string, ACE may need a hash
    set(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIRS})
    set(CMAKE_REQUIRED_LIBRARIES ${ACE_LIBRARIES})
    set(CMAKE_REQUIRED_DEFINITIONS ${YARP_ACE_REQUIRED_DEFINITIONS})
    file(READ ${YARP_MODULE_DIR}/ace_hash_test.cpp YARP_ACE_HAS_STRING_HASH_CPP)
    check_cxx_source_compiles("${YARP_ACE_HAS_STRING_HASH_CPP}" YARP_ACE_HAS_STRING_HASH)
    unset(CMAKE_REQUIRED_INCLUDES)
    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_DEFINITIONS)

endif(SKIP_ACE)
