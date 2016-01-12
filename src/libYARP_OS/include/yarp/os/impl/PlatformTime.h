// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_PLATFORMTIME
#define YARP2_PLATFORMTIME

#include <yarp/conf/system.h>

#ifdef YARP_HAS_ACE
#  include <ace/OS_NS_unistd.h>
#  include <ace/OS_NS_sys_time.h>
#  include <ace/Time_Value.h>
#  include <ace/High_Res_Timer.h>
#  define PLATFORM_TIME_SET(x,y) x.set(y)
#else
#  include <sys/time.h>
#  include <unistd.h>
#  define ACE_Time_Value struct timeval
#  define PLATFORM_TIME_SET(x,y) fromDouble(x,y)
#endif

namespace yarp {
    namespace os {
        namespace impl {

            // defined in src/RFModule.cpp for historical reasons
            void getTime(ACE_Time_Value& now);
            void sleepThread(ACE_Time_Value& sleep_period);
            void addTime(ACE_Time_Value& val, const ACE_Time_Value & add);
            void subtractTime(ACE_Time_Value & val, 
                              const ACE_Time_Value & subtract);
            double toDouble(const ACE_Time_Value &v);
            void fromDouble(ACE_Time_Value &v, double x,int unit=1);
        }
    }
}

#endif
