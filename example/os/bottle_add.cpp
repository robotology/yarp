/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/os/Bottle.h>
using namespace yarp::os;

int main() {
    // create a bottle representing the list (5,"plus",2,"is")
    Bottle b("5 plus 2 is");
    // add an integer that is the sum of element 0 and 2
    b.addInt(b.get(0).asInt()+b.get(2).asInt());
    // print the result -- "result: 5 plus 2 is 7"
    printf("result: %s\n", b.toString().c_str());
    return 0;
}
