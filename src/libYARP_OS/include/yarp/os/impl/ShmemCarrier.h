// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SHMEMCARRIER_
#define _YARP2_SHMEMCARRIER_

#include <yarp/os/impl/AbstractCarrier.h>


//#include <ace/OS.h>

namespace yarp {
    namespace os {
        namespace impl {
            class ShmemCarrier;
        }
    }
}

/**
 * Communicating between two ports via shared memory.
 */
class yarp::os::impl::ShmemCarrier : public AbstractCarrier {
private:
    int version;
public:
    /**
     * verion 1 is "classic" YARP implementation of shmem.
     * version 2 is "Alessandro" version.
     */
    ShmemCarrier(int version = 2);

    virtual Carrier *create();

    virtual String getName();

    virtual int getSpecifierCode();
    virtual bool requireAck();
    virtual bool isConnectionless();
    virtual bool checkHeader(const Bytes& header);
    virtual void getHeader(const Bytes& header);
    virtual void setParameters(const Bytes& header);
    virtual bool respondToHeader(Protocol& proto);
    virtual bool expectReplyToHeader(Protocol& proto);

    /*
      -- unfortunately member templates not working on Visual Studio 6?
    template <class T>
    void becomeShmemVersion(Protocol& proto, bool sender);
    */

    /*
    void becomeShmemVersionTwoWayStream(Protocol& proto, bool sender);
    */

    bool becomeShmemVersionHybridStream(Protocol& proto, bool sender);
    bool becomeShmem(Protocol& proto, bool sender);
};

#endif

