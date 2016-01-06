// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef POLYDRIVERDESCRIPTOR
#define POLYDRIVERDESCRIPTOR

/*
* Copyright (C) 2008 RobotCub Consortium
* Authors: Lorenzo Natale
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

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
