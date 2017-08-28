/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/impl/FakeFace.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/Protocol.h>
#include <yarp/os/impl/FakeTwoWayStream.h>

using namespace yarp::os::impl;
using namespace yarp::os;

bool FakeFace::open(const Contact& address)
{
    YARP_UNUSED(address);
    // happy to open without fuss
    return true;
}

void FakeFace::close()
{
}

InputProtocol *FakeFace::read()
{
    fprintf(stderr, "not implemented\n");
    return nullptr;
}

OutputProtocol *FakeFace::write(const Contact& address)
{
    YARP_UNUSED(address);
    Protocol *prot = new Protocol(new FakeTwoWayStream());
    return prot;
}
