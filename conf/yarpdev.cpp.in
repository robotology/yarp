// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


#include <yarp/os/Network.h>
#include <yarp/dev/Drivers.h>

#include "adder.h"

using namespace yarp::os;
using namespace yarp::dev;

int main(int argc, char *argv[]) {
	Network::init();

    adder();

    int result = Drivers::yarpdev(argc,argv);
	Network::fini();
    
    return result;
}

