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

    include(YarpCheckTypeSize) # regular script does not do C++ types
    set(CMAKE_EXTRA_INCLUDE_FILES ace/config.h ace/String_Base_Const.h)
    set(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIRS})
    set(CMAKE_REQUIRED_LIBRARIES ${ACE_LIBRARIES})
    yarp_check_type_size(ACE_String_Base_Const::size_type SIZE_TYPE)
    unset(CMAKE_EXTRA_INCLUDE_FILES)
    unset(CMAKE_REQUIRED_INCLUDES)
    unset(CMAKE_REQUIRED_LIBRARIES)
    set(YARP_USE_ACE_STRING_BASE_CONST_SIZE_TYPE ${HAVE_SIZE_TYPE})

    if("${ACE_VERSION}" VERSION_LESS "5.4.8")
        set(YARP_ACE_ADDR_HAS_LOOPBACK_METHOD 0)
    else()
        set(YARP_ACE_ADDR_HAS_LOOPBACK_METHOD 1)
    endif()

    # With migration to std::string, ACE may need a hash
    set(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIRS})
    set(CMAKE_REQUIRED_LIBRARIES ${ACE_LIBRARIES})
    file(READ ${YARP_MODULE_DIR}/ace_hash_test.cpp YARP_ACE_HAS_STRING_HASH_CPP)
    check_cxx_source_compiles("${YARP_ACE_HAS_STRING_HASH_CPP}" YARP_ACE_HAS_STRING_HASH)
    unset(CMAKE_REQUIRED_INCLUDES)
    unset(CMAKE_REQUIRED_LIBRARIES)

endif(SKIP_ACE)
