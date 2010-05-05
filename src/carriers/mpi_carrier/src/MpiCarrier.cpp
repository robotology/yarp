// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifdef CREATE_MPI_CARRIER

#include <yarp/os/impl/MpiCarrier.h>
#include <sys/types.h>

using namespace yarp::os::impl;
MpiCarrier::MpiCarrier() : stream(NULL) {    
}

MpiCarrier::~MpiCarrier() {
#ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] Destructor called \n", name.c_str() );
#endif
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

    name = proto.getRoute().getFromName();
    Bytes b2((char*)name.c_str(),name.length());
    proto.os().write(b2);
    proto.os().write('\r');
    proto.os().write('\n');
    
    stream = new MpiStream(name, true);
    char* port = stream->getPortName();
#ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] setting up MpiPort '%s'\n", name.c_str(), port);
#endif

    String uid = stream->getUID();
    Bytes b4((char*)uid.c_str(),uid.length());
    proto.os().write(b4);
    proto.os().write('\r');
    proto.os().write('\n');

    Bytes b3(port,strlen(port));
    proto.os().write(b3);
    proto.os().write('\r');
    proto.os().write('\n');
    proto.os().flush();
#ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] Header sent\n", name.c_str());
#endif

    return proto.os().isOk();
}



 bool MpiCarrier::expectSenderSpecifier(Protocol& proto) {
    // interpret everything that sendHeader wrote
    name = proto.getRoute().getToName();
    stream = new MpiStream(name);
#ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] Waiting for header\n", name.c_str());
#endif
    proto.setRoute(proto.getRoute().addFromName(NetType::readLine(proto.is())));
    String other_id = NetType::readLine(proto.is());
    stream->checkLocal(other_id);
    port = NetType::readLine(proto.is());
#ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] Header received\n", name.c_str());
#endif
    return proto.is().isOk();
}

 bool MpiCarrier::respondToHeader(Protocol& proto) {
    // SWITCH TO NEW STREAM TYPE
#ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] trying to connect to MpiPort '%s'\n", name.c_str(), port.c_str());
#endif
    if (!stream->connect(port))
        return false;
    proto.takeStreams(stream);
#ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] MpiStream successfully setup \n", name.c_str() );
#endif
    return true;
}

 bool MpiCarrier::expectReplyToHeader(Protocol& proto) {
    // SWITCH TO NEW STREAM TYPE
    if (!stream->accept())
        return false;
    proto.takeStreams(stream);
#ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] MpiStream successfully setup \n", name.c_str() );
#endif
    return true;
}


#else

extern "C" int MpiCarrierStatus() {
    return 0;
}


#endif
