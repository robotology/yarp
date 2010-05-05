// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "HumanCarrier.h"

bool HumanCarrier::sendHeader(Protocol& proto) {
    // Send the "magic number" for this carrier
    ManagedBytes header(8);
    getHeader(header.bytes());
    proto.os().write(header.bytes());
    if (!proto.os().isOk()) return false;
    
    // Now we can do whatever we want, as long as somehow
    // we also send the name of the originating port
    
    // let's just send the port name in plain text terminated with a
    // carriage-return / line-feed
    String from = proto.getRoute().getFromName();
    Bytes b2((char*)from.c_str(),from.length());
    proto.os().write(b2);
    proto.os().write('\r');
    proto.os().write('\n');
    proto.os().flush();
    return proto.os().isOk();
}
