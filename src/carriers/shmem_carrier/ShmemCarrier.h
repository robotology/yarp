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


/**
 * Communicating between two ports via shared memory.
 */
class ShmemCarrier : public yarp::os::AbstractCarrier
{
public:
    ShmemCarrier();
    virtual ~ShmemCarrier();

    virtual Carrier* create() override;

    virtual std::string getName() const override;

    virtual int getSpecifierCode();
    virtual bool requireAck() const override;
    virtual bool isConnectionless() const override;
    virtual bool checkHeader(const yarp::os::Bytes& header) override;
    virtual void getHeader(const yarp::os::Bytes& header) override;
    virtual void setParameters(const yarp::os::Bytes& header) override;
    virtual bool respondToHeader(yarp::os::ConnectionState& proto) override;
    virtual bool expectReplyToHeader(yarp::os::ConnectionState& proto) override;

private:
    bool becomeShmemVersionHybridStream(yarp::os::ConnectionState& proto, bool sender);
    bool becomeShmem(yarp::os::ConnectionState& proto, bool sender);
};

#endif // YARP_OS_IMPL_SHMEMCARRIER_H
