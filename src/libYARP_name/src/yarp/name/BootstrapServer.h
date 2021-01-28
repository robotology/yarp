/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPDB_BROADCASTSERVER_INC
#define YARPDB_BROADCASTSERVER_INC

#include <yarp/name/api.h>
#include <yarp/os/Contact.h>
#include <yarp/name/NameService.h>

namespace yarp {
    namespace name {
        class BootstrapServer;
    }
}

/**
 *
 * Multicast and file-based mechanisms for finding the name server.
 *
 */
class YARP_name_API yarp::name::BootstrapServer {
private:
    void *implementation;
public:
    BootstrapServer(NameService& owner);

    virtual ~BootstrapServer();

    static bool configFileBootstrap(yarp::os::Contact& contact,
                                    bool configFileRequired,
                                    bool mayEditConfigFile);

    bool start();
    bool stop();

    yarp::os::Contact where();
};

#endif
