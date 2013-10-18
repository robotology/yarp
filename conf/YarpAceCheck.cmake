# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Alessandro Scalzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


#########################################################################
# Configure ACE

option(SKIP_ACE "Compile YARP without ACE (Linux only, TCP only, limited functionality)" FALSE)
mark_as_advanced(SKIP_ACE)


if(SKIP_ACE)
    set(ACE_LIBRARIES pthread rt)
else()
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
endif()
