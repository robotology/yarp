/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_NAMESTORE_H
#define YARP_OS_NAMESTORE_H

#include <yarp/os/Contact.h>
#include <yarp/os/Bottle.h>

namespace yarp {
    namespace os {
        class NameStore;
    }
}


/**
 * Abstract interface for a database of port names.
 */
class YARP_OS_API yarp::os::NameStore {
public:
    virtual ~NameStore();
    virtual Contact query(const yarp::os::ConstString& name) = 0;
    virtual bool announce(const yarp::os::ConstString& name, int activity) = 0;
    virtual bool process(PortWriter& in,
                         PortReader& out,
                         const Contact& source) = 0;
};


#endif // YARP_OS_NAMESTORE_H
