/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>

#include <cstdio>

using yarp::os::Bottle;

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    // create a bottle representing the list (5,"plus",2,"is")
    Bottle b("5 plus 2 is");
    // add an integer that is the sum of element 0 and 2
    b.addInt32(b.get(0).asInt32() + b.get(2).asInt32());
    // print the result -- "result: 5 plus 2 is 7"
    printf("result: %s\n", b.toString().c_str());
    return 0;
}
