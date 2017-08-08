# Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
# Author: Nicolò Genesio <nicolo.genesio@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# use PARENT_SCOPE to make the following variables available at the 'parent' cmake

set(serverGrabber_srcs src/devices/ServerGrabber/ServerGrabber.cpp  PARENT_SCOPE)
set(serverGrabber_hdrs src/devices/ServerGrabber/ServerGrabber.h PARENT_SCOPE)
set(serverGrabber_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
