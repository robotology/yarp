/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
