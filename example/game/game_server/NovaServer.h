/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef NOVA_SERVER_H
#define NOVA_SERVER_H

#include "NovaClient.h"

class NovaServer {
public:
    NovaServer();
    virtual ~NovaServer();

    int begin(int port);

    void accept(NovaClient& client);

private:
    void *system_resource;
};

#endif
