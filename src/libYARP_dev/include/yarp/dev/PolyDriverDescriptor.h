/*
* Copyright (C) 2008 RobotCub Consortium
* Authors: Lorenzo Natale
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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

#endif
