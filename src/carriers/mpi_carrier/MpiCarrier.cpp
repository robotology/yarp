/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Daniel Krieg <krieg@fias.uni-frankfurt.de>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "MpiCarrier.h"

#include <yarp/os/Route.h>
#include <sys/types.h>

using namespace yarp::os;

MpiCarrier::MpiCarrier() : stream(nullptr), comm(nullptr) {
}

MpiCarrier::~MpiCarrier() {
    yCTrace(MPI_CARRIER, "[MpiCarrier @ %s] Destructor called", route.c_str() );
}

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
    if (!proto.os().isOk()) {
        return false;
    }

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

    if (!MpiControl) {
        return false;
    }
    if (!MpiControl->isRunning()) {
        return false;
    }
    comm->openPort();
    char* port = comm->port_name;
    char* uid = comm->unique_id;

    yCDebug(MPI_CARRIER, "[MpiCarrier @ %s] setting up MpiPort '%s'", route.c_str(), port);

    Bytes b4(uid,strlen(uid));
    proto.os().write(b4);
    proto.os().write('\r');
    proto.os().write('\n');

    Bytes b3(port,strlen(port));
    proto.os().write(b3);
    proto.os().write('\r');
    proto.os().write('\n');
    proto.os().flush();

    yCDebug(MPI_CARRIER, "[MpiCarrier @ %s] Header sent", route.c_str());

    return proto.os().isOk();
}



bool MpiCarrier::expectSenderSpecifier(ConnectionState& proto) {
    // interpret everything that sendHeader wrote
    name = proto.getRoute().getToName();

    yCDebug(MPI_CARRIER, "[MpiCarrier @ %s] Waiting for header", route.c_str());

    other = proto.is().readLine();
    Route r = proto.getRoute();
    r.setFromName(other);
    proto.setRoute(r);

    // Receiver
    route = name + "<-" + other;

    createStream(false);
    if (!MpiControl) {
        return false;
    }
    if (!MpiControl->isRunning()) {
        return false;
    }

    std::string other_id = proto.is().readLine();
    bool notLocal = comm->notLocal(other_id);

    port = proto.is().readLine();

    yCDebug(MPI_CARRIER, "[MpiCarrier @ %s] Header received", route.c_str());

    return notLocal && proto.is().isOk();
}

bool MpiCarrier::respondToHeader(ConnectionState& proto) {
    // SWITCH TO NEW STREAM TYPE
    yCDebug(MPI_CARRIER, "[MpiCarrier @ %s] trying to connect to MpiPort '%s'", route.c_str(), port.c_str());

    if (!comm->connect(port)) {
        delete stream;
        return false;
    }
    proto.takeStreams(stream);

    yCDebug(MPI_CARRIER, "[MpiCarrier @ %s] MpiStream successfully setup", route.c_str() );

    return proto.is().isOk();
}

bool MpiCarrier::expectReplyToHeader(ConnectionState& proto) {
    // SWITCH TO NEW STREAM TYPE
    if (!comm->accept()) {
        delete stream;
        return false;
    }
    proto.takeStreams(stream);

    yCDebug(MPI_CARRIER, "[MpiCarrier @ %s] MpiStream successfully setup", route.c_str() );

    return proto.os().isOk();
}
