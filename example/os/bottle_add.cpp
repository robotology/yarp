/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <yarp/os/Bottle.h>
using namespace yarp::os;

int main() {
    // create a bottle representing the list (5,"plus",2,"is")
    Bottle b("5 plus 2 is");
    // add an integer that is the sum of element 0 and 2
    b.addInt32(b.get(0).asInt32()+b.get(2).asInt32());
    // print the result -- "result: 5 plus 2 is 7"
    printf("result: %s\n", b.toString().c_str());
    return 0;
}
