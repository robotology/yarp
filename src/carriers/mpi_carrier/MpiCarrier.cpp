/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "MpiCarrier.h"

#include <yarp/os/Route.h>
#include <sys/types.h>

using namespace yarp::os;

#ifdef MPI_DEBUG
#include <yarp/os/impl/Logger.h>
using yarp::os::impl::Logger;
#endif

MpiCarrier::MpiCarrier() : stream(nullptr), comm(nullptr) {
    #ifdef MPI_DEBUG
    Logger::get().setVerbosity(1);
    #endif
}

#ifdef MPI_DEBUG
MpiCarrier::~MpiCarrier() {
    printf("[MpiCarrier @ %s] Destructor called \n", route.c_str() );
}
#else
MpiCarrier::~MpiCarrier() = default;
#endif

void  MpiCarrier::getHeader(Bytes& header) const {
    for (size_t i=0; i<8 && i<header.length(); i++) {
        header.get()[i] = target.c_str()[i];
    }
}

bool MpiCarrier::checkHeader(const Bytes& header) {
    if (header.length()!=8) {
        return false;
    }
    for (int i=0; i<8; i++) {
        if (header.get()[i] != target.c_str()[i]) {
            return false;
        }
    }
    return true;
}

bool MpiCarrier::sendHeader(ConnectionState& proto) {
    // Send the "magic number" for this carrier
    ManagedBytes header(8);
    getHeader(header.bytes());
    proto.os().write(header.bytes());
    if (!proto.os().isOk()) return false;

    // Now we can do whatever we want, as long as somehow
    // we also send the name of the originating port

    name = proto.getRoute().getFromName();
    other = proto.getRoute().getToName();
    Bytes b2((char*)name.c_str(),name.length());
    proto.os().write(b2);
    proto.os().write('\r');
    proto.os().write('\n');

    // Sender
    route = name + "->" + other;

    createStream(true);

    if (!MpiControl) return false;
    if (! MpiControl->isRunning())
        return false;
    comm->openPort();
    char* port = comm->port_name;
    char* uid = comm->unique_id;

    #ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] setting up MpiPort '%s'\n", route.c_str(), port);
    #endif

    Bytes b4(uid,strlen(uid));
    proto.os().write(b4);
    proto.os().write('\r');
    proto.os().write('\n');

    Bytes b3(port,strlen(port));
    proto.os().write(b3);
    proto.os().write('\r');
    proto.os().write('\n');
    proto.os().flush();


    #ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] Header sent\n", route.c_str());
    #endif

    return proto.os().isOk();
}



bool MpiCarrier::expectSenderSpecifier(ConnectionState& proto) {
    // interpret everything that sendHeader wrote
    name = proto.getRoute().getToName();

    #ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] Waiting for header\n", route.c_str());
    #endif

    other = proto.is().readLine();
    Route r = proto.getRoute();
    r.setFromName(other);
    proto.setRoute(r);

    // Receiver
    route = name + "<-" + other;

    createStream(false);
    if (!MpiControl) return false;
    if (! MpiControl->isRunning())
        return false;

    std::string other_id = proto.is().readLine();
    bool notLocal = comm->notLocal(other_id);

    port = proto.is().readLine();

    #ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] Header received\n", route.c_str());
    #endif

    return notLocal && proto.is().isOk();
}

bool MpiCarrier::respondToHeader(ConnectionState& proto) {
    // SWITCH TO NEW STREAM TYPE
    #ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] trying to connect to MpiPort '%s'\n", route.c_str(), port.c_str());
    #endif

    if (!comm->connect(port)) {
        delete stream;
        return false;
    }
    proto.takeStreams(stream);

    #ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] MpiStream successfully setup \n", route.c_str() );
    #endif

    return proto.is().isOk();
}

bool MpiCarrier::expectReplyToHeader(ConnectionState& proto) {
    // SWITCH TO NEW STREAM TYPE
    if (!comm->accept()) {
        delete stream;
        return false;
    }
    proto.takeStreams(stream);

    #ifdef MPI_DEBUG
    printf("[MpiCarrier @ %s] MpiStream successfully setup \n", route.c_str() );
    #endif

    return proto.os().isOk();
}
