/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
