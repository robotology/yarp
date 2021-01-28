/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Network.h>
#include <stdio.h>

#include "server.h"

using namespace yarp;
using namespace yarp::os;


int main(int argc, char *argv[])
{
    if (argc<2)
    {
        fprintf(stdout,"port name is missing\n");
        return -1;
    }

    Network yarp;
    if (!yarp.checkNetwork())
        return -2;

    CollatzServer server(argv[1]);

    server.start();

    // wait until a key is pressed
    char ch;
    scanf("%c",&ch);

    server.stop();

    return 0;
}
