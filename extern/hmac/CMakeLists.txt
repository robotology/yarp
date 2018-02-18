# Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# hmac
project(YARP_priv_hmac)

set(hmac_SRCS hmac/hmac_sha2.c
              hmac/sha2.c)

set(hmac_HDRS
    hmac/hmac_sha2.h
    hmac/sha2.h)

add_library(YARP_priv_hmac STATIC ${hmac_SRCS} ${hmac_HDRS})

set_property(TARGET YARP_priv_hmac PROPERTY FOLDER "Libraries/External")

set(hmac_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/hmac PARENT_SCOPE)
set(hmac_LIBRARIES YARP_priv_hmac PARENT_SCOPE)
set(hmac_DEFINITIONS "" PARENT_SCOPE)

install(TARGETS YARP_priv_hmac
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_priv_hmac)
