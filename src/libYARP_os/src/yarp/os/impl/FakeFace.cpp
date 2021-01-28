/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/FakeFace.h>

#include <yarp/os/impl/FakeTwoWayStream.h>
#include <yarp/os/impl/Protocol.h>

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

InputProtocol* FakeFace::read()
{
    fprintf(stderr, "not implemented\n");
    return nullptr;
}

OutputProtocol* FakeFace::write(const Contact& address)
{
    YARP_UNUSED(address);
    auto* prot = new Protocol(new FakeTwoWayStream());
    return prot;
}
