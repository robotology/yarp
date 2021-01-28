/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_PLATFORMLIMITS_H
#define YARP_OS_IMPL_PLATFORMLIMITS_H

#include <yarp/conf/system.h>
#if defined(YARP_HAS_ACE)
#    include <ace/os_include/os_limits.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#else
#    include <climits>
#    if !defined(HOST_NAME_MAX) && defined(__APPLE__)
#        define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#    endif
#endif


#endif // YARP_OS_IMPL_PLATFORMLIMITS_H
