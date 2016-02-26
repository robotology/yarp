/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

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
