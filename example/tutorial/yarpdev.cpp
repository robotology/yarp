/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Network.h>
#include <yarp/dev/Drivers.h>

#include "adder.h"

using namespace yarp::os;
using namespace yarp::dev;

int main(int argc, char *argv[]) {
	Network yarp;

    adder();

    int result = Drivers::yarpdev(argc,argv);
    
    return result;
}

