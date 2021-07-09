/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
