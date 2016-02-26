/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_SHMEMCARRIER
#define YARP2_SHMEMCARRIER

#include <yarp/os/AbstractCarrier.h>


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

    virtual ConstString getName();

    virtual int getSpecifierCode();
    virtual bool requireAck();
    virtual bool isConnectionless();
    virtual bool checkHeader(const Bytes& header);
    virtual void getHeader(const Bytes& header);
    virtual void setParameters(const Bytes& header);
    virtual bool respondToHeader(ConnectionState& proto);
    virtual bool expectReplyToHeader(ConnectionState& proto);

    /*
      -- unfortunately member templates not working on Visual Studio 6?
    template <class T>
    void becomeShmemVersion(ConnectionState& proto, bool sender);
    */

    /*
    void becomeShmemVersionTwoWayStream(ConnectionState& proto, bool sender);
    */

    bool becomeShmemVersionHybridStream(ConnectionState& proto, bool sender);
    bool becomeShmem(ConnectionState& proto, bool sender);
};

#endif

