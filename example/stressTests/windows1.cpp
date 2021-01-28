/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <iostream>
#include <string>

int main()
{
    yarp::os::Network yarp;

    yarp::os::Port port;
    port.open("/test");

    bool done=false;
    while(!done)
    {
        std::cout<<"Type quit to exit\n";
        std::string tmp;
        std::cin>>tmp;
        if (tmp=="quit")
            done=true;
    }

    std::cout<<"You typed exit.\n";

    std::cout<<"Calling Port::close()...";
    port.close();
    std::cout<<"done!\n";
}
