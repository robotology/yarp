/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#include <yarp/os/all.h>
#include <iostream>
using namespace yarp::os;
using namespace yarp::os::impl;

using namespace std;

int main(int argc, char *argv[]) {
    Network yarp;

    std::string name = yarp.getNameServerName();
    Bottle msg, reply;
    msg.addString("bot");
    msg.addString("list");

    Network::write(name.c_str(), msg, reply);

    for(int i=1; i<reply.size(); i++) {
      Property p;
      p.fromString(reply.get(i).toString());

      cout<<p.check("name", Value("[none]")).asString()<<"  "<<endl;
    }

    return 0;
}
