// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NAMESTORE_
#define _YARP2_NAMESTORE_

#include <yarp/os/Contact.h>

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
    virtual ~NameStore() {}
    virtual Contact query(const char *name) = 0;
    virtual bool announce(const char *name, int activity) = 0;
};


#endif
