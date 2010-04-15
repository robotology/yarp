// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifdef CREATE_MPI_CARRIER

#include <yarp/os/impl/MpiCarrier.h>

using namespace yarp::os::impl;
MpiCarrier::MpiCarrier() : stream(NULL) {    
}

MpiCarrier::~MpiCarrier() {
}

void  MpiCarrier::getHeader(const Bytes& header) {
    const char *target = "MPI_____";
    for (int i=0; i<8 && i<header.length(); i++) {
        header.get()[i] = target[i];
    }
}

 bool MpiCarrier::checkHeader(const Bytes& header) {
    if (header.length()!=8) {
        return false;
    }
    const char *target = "MPI_____";
    for (int i=0; i<8; i++) {
        if (header.get()[i] != target[i]) {
            return false;
        }
    }
    return true;
}

 bool MpiCarrier::sendHeader(Protocol& proto) {
    // Send the "magic number" for this carrier
    ManagedBytes header(8);
    getHeader(header.bytes());
    proto.os().write(header.bytes());
    if (!proto.os().isOk()) return false;
    
    // Now we can do whatever we want, as long as somehow
    // we also send the name of the originating port

    String from = proto.getRoute().getFromName();
    Bytes b2((char*)from.c_str(),from.length());
    proto.os().write(b2);
    proto.os().write('\r');
    proto.os().write('\n');
    
    // We need to initialize MPI before first instanciation of MpiStream
    MpiStream::increase_counter();
    stream = new MpiStream(true);
    char* port = stream->getPortName();
    Bytes b3(port,strlen(port));
    proto.os().write(b3);
    proto.os().write('\r');
    proto.os().write('\n');
    proto.os().flush();

    return proto.os().isOk();
}

 bool MpiCarrier::expectSenderSpecifier(Protocol& proto) {
    // interpret everything that sendHeader wrote
    proto.setRoute(proto.getRoute().addFromName(NetType::readLine(proto.is())));
    port = NetType::readLine(proto.is());
    return proto.is().isOk();
}

 bool MpiCarrier::respondToHeader(Protocol& proto) {
    // SWITCH TO NEW STREAM TYPE
    MpiStream::increase_counter();
    stream = new MpiStream();
    stream->connect(port);
    proto.takeStreams(stream);
    return true;
}

 bool MpiCarrier::expectReplyToHeader(Protocol& proto) {
    // SWITCH TO NEW STREAM TYPE
    stream->accept();
    proto.takeStreams(stream);
    return true;
}


#else

extern "C" int MpiCarrierStatus() {
    return 0;
}


#endif
