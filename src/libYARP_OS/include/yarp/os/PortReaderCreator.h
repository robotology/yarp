// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_PORTREADERCREATOR
#define YARP2_PORTREADERCREATOR

#include <yarp/os/ConstString.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/PortReader.h>

namespace yarp {
    namespace os {
        class PortReaderCreator;
    }
}

/**
 *
 * A creator for readers.  This is used when you want a Port to create
 * a different reader for every input connection it receives.  This is
 * a very quick way to make a multi-threaded server that keeps track
 * of which input is which.  Inherit from this class, defining the
 * PortReaderCreator::create method.  Then pass an instance to
 * Port::setReaderCreator.  The create() method will be called every
 * time the Port receives a new connection, and all input coming in
 * via that connection will be channeled appropriately.
 *
 */
class YARP_OS_API yarp::os::PortReaderCreator {
public:

    /**
     * Destructor.
     */
    virtual ~PortReaderCreator();

    /**
     * Factory for PortReader objects
     * @return A new PortReader object -- used to read from a connection to
     * a Port
     */
    virtual PortReader *create() = 0;

};

#endif
