# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_os)

add_subdirectory(src)

# These are used in the YARP_init folder
set(YARP_os_PUBLIC_DEPS ${YARP_os_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_os_PRIVATE_DEPS ${YARP_os_PRIVATE_DEPS} PARENT_SCOPE)
