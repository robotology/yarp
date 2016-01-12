/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
