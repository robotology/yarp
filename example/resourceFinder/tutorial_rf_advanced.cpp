/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/ResourceFinder.h>

#include <string>
#include <iostream>

using namespace yarp::os;

using namespace std;
int main(int argc, char *argv[])
{
    Network yarp;

    ResourceFinder rf;
    rf.setDefaultConfigFile("or.ini");
    rf.setDefaultContext("orBottle");
    rf.configure(argc, argv);

    std::string robotName=rf.find("robot").asString();
    std::string model=rf.findFile("model");

    cout<<"Running with:"<<endl;
    cout<<"robot: "<<robotName.c_str()<<endl;

    if (model=="")
    {
        cout<<"Sorry no model was found, check config parameters"<<endl;
        return -1;
    }

    cout << "Using object model: " << model.c_str() << endl;

    int times=4;
    while(times--)
    {
        cout << ".";
        Time::delay(0.5);
    }

    cout << "done!" <<endl;

    return 0;
}
