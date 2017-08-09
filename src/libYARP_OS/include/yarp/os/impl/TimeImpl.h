/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */



#ifndef YARP_OS_IMPL_TIMEIMPL_H
#define YARP_OS_IMPL_TIMEIMPL_H

#include <yarp/os/Time.h>

namespace yarp {
    namespace os {
        static Clock *pclock = YARP_NULLPTR;
        static yarpClockType yarp_clock_type  = YARP_CLOCK_UNINITIALIZED;

        namespace impl {

            static void removeClock()
            {
                if(yarp::os::pclock != YARP_NULLPTR)
                    delete yarp::os::pclock;
                yarp_clock_type = YARP_CLOCK_UNINITIALIZED;
            }
        }
    }
}

#endif // YARP_OS_IMPL_TIMEIMPL_H