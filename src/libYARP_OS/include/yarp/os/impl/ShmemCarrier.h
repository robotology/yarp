/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_IMPL_SHMEMCARRIER_H
#define YARP_OS_IMPL_SHMEMCARRIER_H

#include <yarp/os/AbstractCarrier.h>

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
class yarp::os::impl::ShmemCarrier : public AbstractCarrier
{
private:
    int version;
public:
    /**
     * version 1 is "classic" YARP implementation of shmem.
     * version 2 is "Alessandro" version.
     */
    ShmemCarrier(int version = 2);

    virtual Carrier *create() override;

    virtual ConstString getName() override;

    virtual int getSpecifierCode();
    virtual bool requireAck() override;
    virtual bool isConnectionless() override;
    virtual bool checkHeader(const Bytes& header) override;
    virtual void getHeader(const Bytes& header) override;
    virtual void setParameters(const Bytes& header) override;
    virtual bool respondToHeader(ConnectionState& proto) override;
    virtual bool expectReplyToHeader(ConnectionState& proto) override;

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

#endif // YARP_OS_IMPL_SHMEMCARRIER_H
