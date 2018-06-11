/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

    virtual Contact query(const std::string& name,
                          const std::string& category = "") = 0;
};

#endif // YARP_OS_CONTACTABLES_H
