/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SHMEM_SHMEMCARRIER_H
#define YARP_SHMEM_SHMEMCARRIER_H

#include <yarp/os/AbstractCarrier.h>


/**
 * Communicating between two ports via shared memory.
 */
class ShmemCarrier : public yarp::os::AbstractCarrier
{
public:
    ShmemCarrier();
    virtual ~ShmemCarrier();

    Carrier* create() const override;

    std::string getName() const override;

    virtual int getSpecifierCode() const;
    bool requireAck() const override;
    bool isConnectionless() const override;
    bool checkHeader(const yarp::os::Bytes& header) override;
    void getHeader(yarp::os::Bytes& header) const override;
    void setParameters(const yarp::os::Bytes& header) override;
    bool respondToHeader(yarp::os::ConnectionState& proto) override;
    bool expectReplyToHeader(yarp::os::ConnectionState& proto) override;

private:
    bool becomeShmemVersionHybridStream(yarp::os::ConnectionState& proto, bool sender);
    bool becomeShmem(yarp::os::ConnectionState& proto, bool sender);
};

#endif // YARP_SHMEM_SHMEMCARRIER_H
