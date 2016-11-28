# use PARENT_SCOPE to make the following variables available at the 'parent' cmake

set(serverInertial_srcs src/devices/ServerInertial/ServerInertial.cpp  PARENT_SCOPE)
set(serverInertial_hdrs src/devices/ServerInertial/ServerInertial.h PARENT_SCOPE)
set(serverInertial_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
