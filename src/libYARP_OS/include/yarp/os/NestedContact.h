/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_NESTEDCONTACT_H
#define YARP_OS_NESTEDCONTACT_H

#include <yarp/os/ConstString.h>

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

    NestedContact(const ConstString& nFullName);

    bool fromString(const ConstString& nFullName);

    void setTypeName(const ConstString& nWireType) {
        wireType = nWireType;
    }

    void setCategoryWrite() {
        category = "+";
    }

    void setCategoryRead() {
        category = "-";
    }

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

    ConstString getTypeName() const {
        return wireType;
    }

    ConstString getTypeNameStar() const {
        return (wireType!="")?wireType:"*";
    }


    bool isNested() const {
        return nestedName!="";
    }

    ConstString toString() const {
        return nestedName + category + "@" + nodeName;
    }

private:
    ConstString fullName;
    ConstString nodeName;
    ConstString nestedName;
    ConstString category;
    ConstString wireType;
};

#endif // YARP_OS_NESTEDCONTACT_H
