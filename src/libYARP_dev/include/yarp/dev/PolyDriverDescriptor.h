/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_POLYDRIVERDESCRIPTOR_H
#define YARP_DEV_POLYDRIVERDESCRIPTOR_H


#include <yarp/dev/PolyDriver.h>
#include <yarp/os/ConstString.h>

namespace yarp {
    namespace dev {
        class PolyDriverDescriptor;
    }
}

class YARP_dev_API yarp::dev::PolyDriverDescriptor
{
public:
    /* pointer to the polydriver */
    PolyDriver *poly;
    /* descriptor */
    yarp::os::ConstString key;

    PolyDriverDescriptor()
    {
        poly=0;
        key="";
    }

    PolyDriverDescriptor(PolyDriver *np, const char *k)
    {
           poly=np;
           key=yarp::os::ConstString(k);
    }
};

#endif // YARP_DEV_POLYDRIVERDESCRIPTOR_H
