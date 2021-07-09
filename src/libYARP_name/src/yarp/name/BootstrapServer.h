/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
