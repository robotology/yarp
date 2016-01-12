/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_CONTACTABLES_H
#define YARP_OS_CONTACTABLES_H

#include <yarp/os/Contactable.h>

namespace yarp {
    namespace os {
        class Contactables;
    }
}

class YARP_OS_API yarp::os::Contactables {
public:
    virtual ~Contactables() {}

    virtual void add(Contactable& contactable) = 0;
    virtual void remove(Contactable& contactable) = 0;

    virtual Contact query(const ConstString& name,
                          const ConstString& category = "") = 0;
};

#endif // YARP_OS_CONTACTABLES_H
