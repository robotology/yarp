// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <stdio.h>
#include <yarp/os/all.h>
using namespace yarp::os;

int main() {
    Network yarp;

    Network::connect("/target/raw/out","/target/raw/in"); // connect ports.
    // can do the same thing from command line with 
    // "yarp connect /target/raw/out /target/raw/in"
    
    return 0;
}
