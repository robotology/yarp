// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_CONTACTABLES_
#define _YARP2_CONTACTABLES_

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

#endif
