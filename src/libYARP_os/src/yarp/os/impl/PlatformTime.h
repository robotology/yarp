/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2011 Anne van Rossum <anne@almende.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_PLATFORMTIME_H
#define YARP_OS_IMPL_PLATFORMTIME_H

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE
#    include <ace/High_Res_Timer.h>
#    include <ace/OS_NS_sys_time.h>
#    include <ace/Time_Value.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#    define PLATFORM_TIME_SET(x, y) x.set(y)
#else
#    include <sys/time.h>
#    define PLATFORM_TIME_SET(x, y) fromDouble(x, y)
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
void addTime(YARP_timeval& val, const YARP_timeval& add);
void subtractTime(YARP_timeval& val,
                  const YARP_timeval& subtract);
double toDouble(const YARP_timeval& v);
void fromDouble(YARP_timeval& v, double x, int unit = 1);

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_PLATFORMTIME_H
