/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_POLYDRIVERDESCRIPTOR_H
#define YARP_DEV_POLYDRIVERDESCRIPTOR_H


#include <yarp/dev/PolyDriver.h>
#include <string>

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
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) key;

    PolyDriverDescriptor()
    {
        poly=0;
        key="";
    }

    PolyDriverDescriptor(PolyDriver *np, const char *k)
    {
           poly=np;
           key=std::string(k);
    }
};

#endif // YARP_DEV_POLYDRIVERDESCRIPTOR_H
