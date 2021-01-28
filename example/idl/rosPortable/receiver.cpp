/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/rosmsg/SharedData.h>
#include <iostream>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>

using namespace std;

int main()
{
    yarp::os::Network network;

    cout<<"Starting receiver\n";

    yarp::os::Port port;
    if (!port.open("/receiver"))
    {
        cerr<<"Error opening port, check your yarp network\n";
        return -1;
    }

    network.connect("/sender", "/receiver");

    int count=0;
    while(true)
    {
        yarp::rosmsg::SharedData d;
        port.read(d);

        //access d
        cout << count << " Received SharedData:\n";
        cout << d.text << "\n";
        for (int i=0; i<d.content.size(); i++)
        {
            cout<<d.content[i]<<" ";
        }
        cout<<"\n";
        count++;
    }

    return 0;
}
