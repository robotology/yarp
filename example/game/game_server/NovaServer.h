// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

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
