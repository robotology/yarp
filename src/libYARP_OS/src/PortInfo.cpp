// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/PortInfo.h>


yarp::os::PortInfo::PortInfo() :
    tag(PORTINFO_NULL),
    incoming(false),
    created(true),
    message("no information") {
}