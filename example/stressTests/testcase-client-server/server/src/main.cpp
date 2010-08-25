/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Network.h>
#include <stdio.h>

#include "server.hpp"

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


