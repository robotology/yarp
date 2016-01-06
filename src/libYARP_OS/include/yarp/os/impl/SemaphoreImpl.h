// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2009 RobotCub Consortium
 * Authors: Miguel Sarabia del Castillo, Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_SEMAPHOREIMPL
#define YARP2_SEMAPHOREIMPL


// Pre-declare semaphore
namespace yarp {
    namespace os {
        namespace impl {
            class SemaphoreImpl;
        }
    }
}

//Decide which implementation to use
#include <yarp/conf/system.h>

#ifdef YARP_HAS_CXX11
#  include <yarp/os/impl/CXX11SemaphoreImpl.h>
#else
#  ifdef __linux__
// There is a problem with YARP+ACE semaphores on some Linux distributions,
// where semaphores fail to work correctly.  Workaround.
#    include <yarp/os/impl/POSIXSemaphoreImpl.h>
#  else
// On Mac, POSIX semaphores are just too burdensome
#    ifdef __APPLE__
#      include <yarp/os/impl/MachSemaphoreImpl.h>
#    else
//   For everything else, there's ACE
#      ifdef YARP_HAS_ACE
#        include <yarp/os/impl/ACESemaphoreImpl.h>
#      else
#        error Cannot implement semaphores
#      endif
#    endif
#  endif
#endif 

#endif

