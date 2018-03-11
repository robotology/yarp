/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
     * verion 1 is "classic" YARP implementation of shmem.
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
