// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_OS_PORTABLE_
#define _YARP2_OS_PORTABLE_

#include <yarp/os/PortReader.h>
#include <yarp/os/PortWriter.h>

namespace yarp {
    namespace os {
        class Portable;
    }
}

/**
 * \ingroup comm_class
 *
 * This is a base class for objects that can be both read from
 * and be written to the YARP network.  It is a simple union of
 * PortReader and PortWriter.
 */
class YARP_OS_API yarp::os::Portable : public PortReader, public PortWriter
{
public:
    // reiterate the key inherited virtual methods, just as a reminder

    virtual bool read(ConnectionReader& connection) = 0;
    virtual bool write(ConnectionWriter& connection) = 0;

    virtual Type getType() { return getReadType(); }
};

#endif
