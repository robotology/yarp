// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/name/NameService.h>
#include <yarp/name/NameServerConnectionHandler.h>
#include <yarp/os/DummyConnector.h>

using namespace yarp::name;
using namespace yarp::os;

bool NameService::process(yarp::os::PortWriter& in, 
                          yarp::os::PortReader& out, 
                          const yarp::os::Contact& remote) {
    DummyConnector din, dout;
    din.setTextMode(true);
    dout.setTextMode(true);
    in.write(din.getWriter());
    NameServerConnectionHandler handler(this);
    bool ok = handler.apply(din.getReader(),&dout.getWriter(),false);
    out.read(dout.getReader());
    return ok;
}
