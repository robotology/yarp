// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NESTEDCONTACT_
#define _YARP2_NESTEDCONTACT_

#include <yarp/os/Contact.h>

namespace yarp {
    namespace os {
        class NestedContact;
    }
}

/**
 *
 * A placeholder for rich contact information.  Includes types, and
 * can express a ROS-style node/topic or node/service combination.
 *
 */
class YARP_OS_API yarp::os::NestedContact {
public:
    NestedContact() {}

    NestedContact(const char *nFullName);

    bool fromString(const char *nFullName);

    ConstString getFullName() const {
        return fullName;
    }
    ConstString getNodeName() const {
        return nodeName;
    }

    ConstString getNestedName() const {
        return nestedName;
    }

    ConstString getCategory() const {
        return category;
    }

    bool isNested() const {
        return nestedName!="";
    }
    
private:
    ConstString fullName;
    ConstString nodeName;
    ConstString nestedName;
    ConstString category;
};

#endif

