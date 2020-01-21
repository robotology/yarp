/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <stdio.h>

using namespace yarp::os;

class Input : public BufferedPort<Bottle> {
public:
    virtual void onRead(Bottle& datum) {
        printf("Got a bottle containing: %s\n", datum.toString().c_str());
    }
};

int main() {
    Network yarp;
    Network::setLocalMode(true); // don't actually need a name server

    Input in;
    Port out;
    in.open("/in");
    in.useCallback(); // input should go to onRead() callback
    out.open("/out");

    Network::connect("/out","/in");
    Bottle b("10 10 20");
    out.write(b);

    return 0;
}
