// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YARPDB_BROADCASTSERVER_INC
#define YARPDB_BROADCASTSERVER_INC

#include <yarp/os/Contact.h>

#include "NameService.h"

/**
 *
 * Multicast and file-based mechanisms for finding the name server.
 *
 */
class BootstrapServer {
private:
    void *implementation;
public:
    BootstrapServer(NameService& owner);

    virtual ~BootstrapServer();

    static bool configFileBootstrap(yarp::os::Contact& contact, bool autofix = true);

    bool start();
    bool stop();

    yarp::os::Contact where();
};

#endif

