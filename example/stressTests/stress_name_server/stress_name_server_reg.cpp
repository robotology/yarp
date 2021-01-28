/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/all.h>

using namespace yarp::os;

int main(int argc, char *argv[]) {
    Network yarp;

    char buf[256];
    int top = 500;
    for (int i=0; i<top; i++) {
        sprintf(buf,"/stress/%06d",i);
        yarp.registerName(buf);
    }
    for (int i=0; i<top; i++) {
        sprintf(buf,"/stress/%06d",i);
        yarp.queryName(buf);
    }
    for (int i=0; i<top; i++) {
        sprintf(buf,"/stress/%06d",i);
        yarp.unregisterName(buf);
    }

    return 0;
}
