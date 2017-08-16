/*
 * Copyright (C) 2011 Istituto Italiano di Tecnologia (IIT), Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP_OS_IMPL_PLATFORMTIME_H
#define YARP_OS_IMPL_PLATFORMTIME_H

#include <yarp/conf/system.h>
#include <yarp/os/impl/PlatformUnistd.h>
#ifdef YARP_HAS_ACE
#  include <ace/OS_NS_sys_time.h>
#  include <ace/Time_Value.h>
#  include <ace/High_Res_Timer.h>
#  define PLATFORM_TIME_SET(x, y) x.set(y)
#else
#  include <sys/time.h>
#  define ACE_Time_Value struct timeval
#  define PLATFORM_TIME_SET(x, y) fromDouble(x, y)
#endif

namespace yarp {
    namespace os {
        namespace impl {

#ifdef YARP_HAS_ACE
            typedef ACE_Time_Value YARP_timeval;
#else
            typedef struct timeval YARP_timeval;
#endif

            void getTime(YARP_timeval& now);
            void sleepThread(YARP_timeval& sleep_period);
            void addTime(YARP_timeval& val, const YARP_timeval & add);
            void subtractTime(YARP_timeval & val,
                              const YARP_timeval & subtract);
            double toDouble(const YARP_timeval &v);
            void fromDouble(YARP_timeval &v, double x, int unit=1);
        }
    }
}

#endif // YARP_OS_IMPL_PLATFORMTIME_H
