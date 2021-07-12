/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
