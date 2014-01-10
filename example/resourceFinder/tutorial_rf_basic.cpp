// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2012 iCub Facility, Istituto Italiano di Tecnologia
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
    rf.setDefaultConfigFile("random.ini");
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
