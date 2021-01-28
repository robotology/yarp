/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/ManagedBytes.h>
#include "HumanCarrier.h"

bool HumanCarrier::sendHeader(ConnectionState& proto) {
    // Send the "magic number" for this carrier
    ManagedBytes header(8);
    getHeader(header.bytes());
    proto.os().write(header.bytes());
    if (!proto.os().isOk()) return false;

    // Now we can do whatever we want, as long as somehow
    // we also send the name of the originating port

    // let's just send the port name in plain text terminated with a
    // carriage-return / line-feed
    std::string from = proto.getRoute().getFromName();
    Bytes b2((char*)from.c_str(),from.length());
    proto.os().write(b2);
    proto.os().write('\r');
    proto.os().write('\n');
    proto.os().flush();
    return proto.os().isOk();
}
