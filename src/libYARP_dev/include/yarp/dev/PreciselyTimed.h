// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Robotcub Consortium
 * Author Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#ifndef __IPRECISELYTIMED__
#define __IPRECISELYTIMED__

#include <yarp/os/Stamp.h>

namespace yarp{
    namespace dev{
        class IPreciselyTimed;
    }
}

class yarp::dev::IPreciselyTimed {
public:
    /**
    * Return the time stamp relative to the last acquisition.
    */ 
	virtual yarp::os::Stamp getLastInputStamp() = 0;
	virtual ~IPreciselyTimed(){}
};

#endif

