// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/FakeFace.h>
#include <yarp/Logger.h>
#include <yarp/Protocol.h>
#include <yarp/FakeTwoWayStream.h>

using namespace yarp;

static Logger fakeFaceLog("FakeFace", Logger::get());

void FakeFace::open(const Address& address) {
    // happy to open without fuss
}

void FakeFace::close() {
}

InputProtocol *FakeFace::read() {
    fakeFaceLog.fail("not yet implemented");
    return NULL;
}

OutputProtocol *FakeFace::write(const Address& address) {
    Protocol *prot = new Protocol(new FakeTwoWayStream());
    return prot;
}

