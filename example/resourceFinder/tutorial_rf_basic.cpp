/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>

#include <string>
#include <iostream>

using namespace yarp::os;

using namespace std;
int main(int argc, char *argv[]) 
{
    Network yarp;

    ResourceFinder rf;
    rf.setVerbose();
    rf.setDefaultConfigFile("config.ini");
    rf.setDefaultContext("randomMotion");
    rf.configure(argc, argv);
        
    ConstString robotName=rf.find("robot").asString();
    ConstString partName=rf.find("part").asString();
    int joint=rf.find("joint").asInt();

    cout<<"Running with:"<<endl;
    cout<<"robot: "<<robotName.c_str()<<endl;
    cout<<"part: "<<partName.c_str()<<endl;
    cout<<"joint: "<<joint<<endl;
       
    return 0;
}
