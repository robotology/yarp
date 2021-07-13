# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# hmac
project(YARP_priv_hmac)

add_library(YARP_priv_hmac STATIC)

set(hmac_SRCS
  hmac/hmac_sha2.c
  hmac/sha2.c
)

set(hmac_HDRS
  hmac/hmac_sha2.h
  hmac/sha2.h
)

target_sources(YARP_priv_hmac PRIVATE ${hmac_SRCS} ${hmac_HDRS})

set_property(TARGET YARP_priv_hmac PROPERTY FOLDER "Libraries/External")

set(hmac_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/hmac PARENT_SCOPE)
set(hmac_LIBRARIES YARP_priv_hmac PARENT_SCOPE)
set(hmac_DEFINITIONS "" PARENT_SCOPE)

install(
  TARGETS YARP_priv_hmac
  EXPORT YARP_priv_hmac
  COMPONENT YARP_priv_hmac
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

if(NOT CREATE_SHARED_LIBS)
  include(YarpInstallBasicPackageFiles)
  yarp_install_basic_package_files(YARP_priv_hmac)
endif()
