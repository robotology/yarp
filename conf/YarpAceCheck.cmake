# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Alessandro Scalzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


#########################################################################
# Configure ACE

option(SKIP_ACE "Compile YARP without ACE (Linux only, TCP only, limited functionality" FALSE)
mark_as_advanced(SKIP_ACE)


if(SKIP_ACE)
    set(ACE_LIBRARIES pthread rt)
else(SKIP_ACE)
    set(YARP_HAS_ACE 1)

    find_package(ACE REQUIRED)

    include(YarpCheckTypeSize) # regular script does not do C++ types
    set(CMAKE_EXTRA_INCLUDE_FILES ace/config.h ace/String_Base_Const.h)
    set(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIR} ${ACE_INCLUDE_CONFIG_DIR})
    set(CMAKE_REQUIRED_LIBRARIES ${ACE_LIBRARIES})
    yarp_check_type_size(ACE_String_Base_Const::size_type SIZE_TYPE)
    set(CMAKE_EXTRA_INCLUDE_FILES)
    set(CMAKE_REQUIRED_INCLUDES)
    set(CMAKE_REQUIRED_LIBRARIES)
    set(YARP_USE_ACE_STRING_BASE_CONST_SIZE_TYPE ${HAVE_SIZE_TYPE})

    include(YarpCheckStructHasMember)
    set(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIR} ${ACE_INCLUDE_CONFIG_DIR})
    set(CMAKE_REQUIRED_LIBRARIES ${ACE_LIBRARIES})
    yarp_check_struct_has_member("ACE_INET_Addr" is_loopback ace/INET_Addr.h YARP_ACE_ADDR_HAS_LOOPBACK_METHOD)
    set(CMAKE_EXTRA_INCLUDE_FILES)
    set(CMAKE_REQUIRED_LIBRARIES)

    # From YARP 6.0.2 on, __ACE_INLINE__ is needed
    include (CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIR} ${ACE_INCLUDE_CONFIG_DIR})
    set(CMAKE_REQUIRED_LIBRARIES ${ACE_LIBRARIES})
    file(READ ${YARP_MODULE_DIR}/ace_test.cpp YARP_ACE_NEEDS_INLINE_CPP)
    check_cxx_source_compiles("${YARP_ACE_NEEDS_INLINE_CPP}" YARP_ACE_COMPILES_WITHOUT_INLINE)
    set(CMAKE_EXTRA_INCLUDE_FILES)
    set(CMAKE_REQUIRED_LIBRARIES)

    # With migration to std::string, ACE may need a hash
    set(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIR} ${ACE_INCLUDE_CONFIG_DIR})
    set(CMAKE_REQUIRED_LIBRARIES ${ACE_LIBRARIES})
    file(READ ${YARP_MODULE_DIR}/ace_hash_test.cpp YARP_ACE_HAS_STRING_HASH_CPP)
    check_cxx_source_compiles("${YARP_ACE_HAS_STRING_HASH_CPP}" YARP_ACE_HAS_STRING_HASH)
    set(CMAKE_EXTRA_INCLUDE_FILES)
    set(CMAKE_REQUIRED_LIBRARIES)

    if(NOT YARP_ACE_COMPILES_WITHOUT_INLINE)
        add_definitions(-D__ACE_INLINE__)
    endif(NOT YARP_ACE_COMPILES_WITHOUT_INLINE)

endif(SKIP_ACE)
