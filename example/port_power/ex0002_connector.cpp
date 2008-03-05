// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

int main() {
    Network yarp;

    Network::connect("/out","/in");  // connect two ports.
    // can do the same thing from command line with "yarp connect /out /in"
    
    return 0;
}
