/*
 * Copyright (C) 2011 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <yarp/os/Network.h>
#include <yarp/serversql/yarpserversql.h>

int main(int argc, char *argv[])
{
    // Yarp server must always run using system clock
    yarp::os::Network::setEnvironment(yarp::os::ConstString("YARP_CLOCK"), yarp::os::ConstString(""));
    yarp::os::Network yarp;
    int ret=yarpserver_main(argc, argv);
    return (ret!=0?1:0);
}
