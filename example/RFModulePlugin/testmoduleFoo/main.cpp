/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <module.h>
#include <yarp/os/Network.h>
int main(int argc, char** argv)
{
    yarp::os::Network yarp;
    moduleFoo module;
    return module.runModule();
}
